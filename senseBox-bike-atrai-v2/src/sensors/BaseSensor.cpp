#include "BaseSensor.h"

BaseSensor::BaseSensor(const char *taskName, uint32_t taskStackSize, uint32_t taskDelay)
    : activeSubscription(false), sendBLE(false), taskStackSize(taskStackSize), taskDelay(taskDelay) {}

void BaseSensor::begin()
{
    initSensor();
    delay(1000);
    xTaskCreate(sensorTask, taskName, taskStackSize, this, 1, NULL);
}

void BaseSensor::subscribe(std::function<void(std::vector<float>)> callback)
{
    this->measurementCallback = callback;
}

void BaseSensor::startSubscription()
{
    activeSubscription = true;
}

void BaseSensor::stopSubscription()
{
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
            sensor->readSensorData();
        }
        vTaskDelay(pdMS_TO_TICKS(sensor->taskDelay));
    }
}