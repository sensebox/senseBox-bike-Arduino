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
#include "src/BLEModule.h"

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
extern "C" void app_main(void)
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
    char dir[64];
    // TODO: as we are testing multiple models we might want to include them in a smarter way. Is there something like command line arguments?
    snprintf(dir, sizeof(dir), "%s/espdl_models", CONFIG_BSP_SD_MOUNT_POINT);
    dl::Model* model = new dl::Model((const char *)espdl_model, dir);
    while(1==1) {
        ESP_LOGI(TAG, "Free heap: %lu", esp_get_free_heap_size()); // TODO: small memory leak somewhere in the loop...
        camera_fb_t *pic = esp_camera_fb_get();

        // use pic->buf to access the image
        ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes. Its width was: %zu. Its height was: %zu", pic->len, pic->width, pic->height);
        
        dl::image::jpeg_img_t jpeg_img = {
            .data = (uint8_t *) pic->buf,
            .width = (int) pic->width,
            .height = (int) pic->height,
            .data_size = (uint32_t)(pic->len),
        };

        dl::image::img_t img;
        img.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
        sw_decode_jpeg(jpeg_img, img, true);
        esp_camera_fb_return(pic);

        uint32_t t0, t1;
        float delta;
        t0 = esp_timer_get_time();

        dl::image::ImagePreprocessor* m_image_preprocessor = new dl::image::ImagePreprocessor(model, {123.675, 116.28, 103.53}, {58.395, 57.12, 57.375}); // TODO: the mean and std need to be adjusted for our usecase
        ESP_LOGI(TAG, "picture width and height: %zu %zu", img.width, img.height);
        m_image_preprocessor->preprocess(img);

        model->run();
        const int check = 5;
        ModelPostProcessor m_postprocessor(model, check, std::numeric_limits<float>::lowest(), true);
        std::vector<dl::cls::result_t> &results = m_postprocessor.postprocess();

        t1 = esp_timer_get_time();
        delta = t1 - t0;
        printf("Inference in %8.0f us.", delta);
        
        for (const auto &res : results) {
            ESP_LOGI(TAG, "category: %s, score: %f\n", res.cat_name, res.score);
        }
        heap_caps_free(img.data);

        uint8_t score0 = static_cast<uint8_t>(results[0].score * 100);
        uint8_t score1 = static_cast<uint8_t>(results[1].score * 100);
        uint8_t score2 = static_cast<uint8_t>(results[2].score * 100);
        uint8_t score3 = static_cast<uint8_t>(results[3].score * 100);
        uint8_t score4 = static_cast<uint8_t>(results[4].score * 100);

        notify_surface_classification(
            &score0,
            &score1,
            &score2,
            &score3,
            &score4
        );

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
