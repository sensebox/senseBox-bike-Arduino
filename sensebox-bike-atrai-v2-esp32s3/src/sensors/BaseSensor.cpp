#include "BaseSensor.h"

BaseSensor::BaseSensor(const char *taskName, uint32_t taskStackSize, uint32_t taskDelay, uint32_t taskPriority, uint16_t core)
    : activeSubscription(false), sendBLE(false), taskStackSize(taskStackSize), taskDelay(taskDelay), taskPriority(taskPriority), core(core) {}
    
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
        activeSubscription = true;
        // https://docs.espressif.com/projects/esp-idf/en/v5.2.3/esp32s3/api-guides/performance/speed.html#choosing-task-priorities-of-the-application
        xTaskCreatePinnedToCore(sensorTask, taskName, taskStackSize, this, taskPriority, &this->taskHandle, core);
        // very important tasks could run on priority 20 on core 0
    }
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
        unsigned long prevTime = millis();
        if (sensor->activeSubscription)
        {
            if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE)
            {
                sensor->readSensorData();
                xSemaphoreGive(i2c_mutex);
            }
        }

        // if (sensor->taskDelay > 0 && (millis() - prevTime) < sensor->taskDelay)
        // {
        //     vTaskDelay(pdMS_TO_TICKS(sensor->taskDelay - (millis() - prevTime)));
        // }
        vTaskDelay(pdMS_TO_TICKS(sensor->taskDelay));
    }
}