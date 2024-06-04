#include "DistanceSensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <vl53l8cx_class.h>

const String DistanceSensor::distanceUUID = "B3491B60C0F34306A30D49C91F37A62B";

VL53L8CX sensor_vl53l8cx_top(&Wire, -1, -1);

int distanceCharacteristic = 0;

void DistanceSensor::begin()
{
  Wire.begin();
  Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz
  sensor_vl53l8cx_top.begin();
  sensor_vl53l8cx_top.init_sensor();
  sensor_vl53l8cx_top.vl53l8cx_set_ranging_frequency_hz(30);
  sensor_vl53l8cx_top.vl53l8cx_set_resolution(VL53L8CX_RESOLUTION_8X8);
  sensor_vl53l8cx_top.vl53l8cx_start_ranging();

  distanceCharacteristic = BLEModule::createCharacteristic(distanceUUID.c_str());

  sendBLE = false;
  activeSubscription = true;
  xTaskCreate(sensorTask, "DistanceSensorTask", 8192, this, 1, NULL);
}

void DistanceSensor::subscribe(std::function<void(String, std::vector<float>)> callback)
{
  this->measurementCallback = callback;
}

void DistanceSensor::startSubscription()
{
  activeSubscription = true;
}

void DistanceSensor::stopSubscription()
{
  activeSubscription = false;
}

void DistanceSensor::startBLE()
{
  setBLEStatus(true);
}

void DistanceSensor::stopBLE()
{
  setBLEStatus(false);
}

void DistanceSensor::sensorTask(void *pvParameters)
{
  DistanceSensor *sensor = static_cast<DistanceSensor *>(pvParameters);

  VL53L8CX_ResultsData Results;
  uint8_t NewDataReady = 0;
  uint8_t status;

  while (true)
  {
    if (sensor->activeSubscription)
    {
      float oldVl53l8cxMin = -1.0;
      status = sensor_vl53l8cx_top.vl53l8cx_check_data_ready(&NewDataReady);

      if ((!status) && (NewDataReady != 0))
      {
        sensor_vl53l8cx_top.vl53l8cx_get_ranging_data(&Results);
        float min = 10000.0;
        for (int i = 0; i < VL53L8CX_RESOLUTION_8X8 * VL53L8CX_NB_TARGET_PER_ZONE; i++)
        {
          if ((&Results)->target_status[i] != 255)
          {
            float distance = (&Results)->distance_mm[i];
            if (min > distance)
            {
              min = distance;
            }
          }
        }
        oldVl53l8cxMin = (min == 10000.0) ? 0.0 : min;
      }

      float distance = oldVl53l8cxMin / 10.0;

      if (sensor->measurementCallback)
      {
        sensor->measurementCallback(sensor->uuid, {distance});
      }

      if (sensor->sendBLE)
      {
        sensor->notifyBLE(distance);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void DistanceSensor::notifyBLE(float distance)
{
  BLEModule::writeBLE(distanceCharacteristic, distance);
}
