#include "esp_log.h"
#include "dl_model_base.hpp"
#include "dl_image_define.hpp"
#include "dl_image_preprocessor.hpp"
#include "dl_cls_postprocessor.hpp"
#include "dl_image_jpeg.hpp"
#include "bsp/esp-bsp.h"
#include <esp_system.h>
#include <nvs_flash.h>
#include <string.h>
#include <sys/param.h>
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "host/ble_hs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "include/ModelPostProcessor.hpp"
#include "include/BLEModule.h"
#include "include/surface_classification.hpp"
#include "include/particulate_matter.hpp"
#include <sps30.h>

unsigned long interval_timer = 0;

// BLEModule bleModule;
int temperatureCharacteristic = 0;

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif
#include "esp_camera.h"

extern const uint8_t espdl_model[] asm("_binary_raw_mobilenet_espdl_start");

const char *TAG = "surface_classifier";

static const char *device_name = "senseBox:bike[XXX]";

#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  4
#define SIOC_GPIO_NUM  5

#define Y9_GPIO_NUM    16
#define Y8_GPIO_NUM    17
#define Y7_GPIO_NUM    18
#define Y6_GPIO_NUM    12
#define Y5_GPIO_NUM    10
#define Y4_GPIO_NUM    8
#define Y3_GPIO_NUM    9
#define Y2_GPIO_NUM    11
#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM  7
#define PCLK_GPIO_NUM  13

// Camera Module pin mapping
static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,

    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,

    // XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    // Change to PIXFORMAT_RGB888
    .pixel_format = PIXFORMAT_JPEG,  // YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_240X240, // QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 12, // 0-63 lower number means higher quality
    .fb_count = 1,      // if more than one, i2s runs in continuous mode. Use only with JPEG
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init_camera(void)
{
    // initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}

extern "C" {
    void app_main(void);
    void getSPS30Data();
}

void sps30_task(void *pvParameters) {
    while (1) {
        float pm1_0, pm2_5, pm4_0, pm10;
        if (read_sps30(pm1_0, pm2_5, pm4_0, pm10)) {
            ESP_LOGI(TAG, "PM1.0: %f PM2.5: %f PM4.0: %f PM10: %f", pm1_0, pm2_5, pm4_0, pm10);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void surface_classification_task(void *pvParameters) {
    dl::Model* model = (dl::Model*)pvParameters;
    unsigned long interval_timer = 0;
    while (1) {
        ESP_LOGI(TAG, "loop repeats every %ld ms", millis() - interval_timer);
        interval_timer = millis();

        ESP_LOGI(TAG, "Free heap: %lu", esp_get_free_heap_size());
        camera_fb_t *pic = esp_camera_fb_get();
        ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes. Its width was: %zu. Its height was: %zu", pic->len, pic->width, pic->height);

        dl::image::img_t img = decode_jpeg_to_img(pic);
        esp_camera_fb_return(pic);

        std::vector<dl::cls::result_t> results = run_inference(model, img, TAG);

        for (const auto &res : results) {
            ESP_LOGI(TAG, "category: %s, score: %f\n", res.cat_name, res.score);
        }
        heap_caps_free(img.data);

        uint8_t scores[5] = {
            static_cast<uint8_t>(results[0].score * 100),
            static_cast<uint8_t>(results[1].score * 100),
            static_cast<uint8_t>(results[2].score * 100),
            static_cast<uint8_t>(results[3].score * 100),
            static_cast<uint8_t>(results[4].score * 100)
        };

        notify_surface_classification(scores);

        vTaskDelay(2000 / portTICK_PERIOD_MS); // Adjust delay as needed
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        MODLOG_DFLT(ERROR, "Failed to init nimble %d \n", ret);
        return;
    }

    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.reset_cb = on_reset;

    int rc = gatt_svr_init();
    assert(rc == 0);

    /* Set the default device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);

    /* Start the task */
    nimble_port_freertos_init(host_task);

    if (ESP_OK != init_camera()) {
        return;
    }
    Serial.begin(115200);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // wait for serial to be ready
    init_sps30();

    // Model initialization
    char dir[64];
    snprintf(dir, sizeof(dir), "%s/espdl_models", CONFIG_BSP_SD_MOUNT_POINT);
    dl::Model* model = new dl::Model((const char *)espdl_model, dir);

    // Create SPS30 task on core 0
    xTaskCreatePinnedToCore(
        sps30_task,           // Task function
        "sps30_task",         // Name
        4096,                 // Stack size
        NULL,                 // Parameter
        3,                    // Priority
        NULL,                 // Task handle
        1                     // Core 0
    );

    // Create surface classification task on core 1
    xTaskCreatePinnedToCore(
        surface_classification_task,
        "surface_classification_task",
        8192*2,
        model,
        20,
        NULL,
        0                    // Core 1
    );

    // app_main can return or do other minimal work
}
