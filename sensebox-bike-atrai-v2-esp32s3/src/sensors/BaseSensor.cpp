#include "BaseSensor.h"

BaseSensor::BaseSensor(const char *taskName, uint32_t taskStackSize, uint32_t taskDelay)
    : activeSubscription(false), sendBLE(false), taskStackSize(taskStackSize), taskDelay(taskDelay) {}
    
static SemaphoreHandle_t i2c_mutex;

void BaseSensor::begin()
{
    i2c_mutex = xSemaphoreCreateMutex();
    initSensor();
    delay(500);
    // xTaskCreate(sensorTask, taskName, taskStackSize, this, 1, NULL);
}

void BaseSensor::subscribe(std::function<void(std::vector<float>)> callback)
{
    this->measurementCallback = callback;
}

void BaseSensor::startSubscription()
{
    if (this->taskHandle == NULL)
    {
        // https://docs.espressif.com/projects/esp-idf/en/v5.2.3/esp32s3/api-guides/performance/speed.html#choosing-task-priorities-of-the-application
        xTaskCreate(sensorTask, taskName, taskStackSize, this, 17, &this->taskHandle);
        // very important tasks could run on priority 20 on core 0
    }
    activeSubscription = true;
}

void BaseSensor::stopSubscription()
{
    if (this->taskHandle != NULL)
    {
        vTaskDelete(this->taskHandle);
    }
    activeSubscription = false;
}

void BaseSensor::startBLE()
{
    sendBLE = true;
}

void BaseSensor::stopBLE()
{
    sendBLE = false;
}

void BaseSensor::sensorTask(void *pvParameters)
{
    BaseSensor *sensor = static_cast<BaseSensor *>(pvParameters);
    while (true)
    {
        if (sensor->activeSubscription)
        {
            if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE)
            {
                sensor->readSensorData();
                xSemaphoreGive(i2c_mutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(sensor->taskDelay));
    }
}