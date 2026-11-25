#include "BaseSensor.h"

BaseSensor::BaseSensor(const char *taskName, uint32_t taskStackSize, uint32_t taskDelay)
    : activeSubscription(false), sendBLE(false), taskStackSize(taskStackSize), taskDelay(taskDelay) {}

// 77 for sensebox multiplexer, 70 for adafruit multiplexer
#define TCAADDR 0x70

void BaseSensor::tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void BaseSensor::begin()
{
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