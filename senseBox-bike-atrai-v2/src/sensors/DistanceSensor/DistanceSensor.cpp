#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() : BaseSensor("distanceTask", 8192, 100) {}

String distanceUUID = "B3491B60C0F34306A30D49C91F37A62B";
int distanceCharacteristic = 0;

VL53L8CX sensor_vl53l8cx_top(&Wire, -1, -1);

void DistanceSensor::initSensor()
{
  sensor_vl53l8cx_top.init_sensor();
  sensor_vl53l8cx_top.vl53l8cx_set_ranging_frequency_hz(30);
  sensor_vl53l8cx_top.vl53l8cx_set_resolution(VL53L8CX_RESOLUTION_8X8);
  sensor_vl53l8cx_top.vl53l8cx_start_ranging();

  distanceCharacteristic = BLEModule::createCharacteristic(distanceUUID.c_str());
}

void DistanceSensor::readSensorData()
{
  Serial.println("Reading distance sensor data...");

  VL53L8CX_ResultsData Results;
  uint8_t NewDataReady = 0;
  uint8_t status = sensor_vl53l8cx_top.vl53l8cx_check_data_ready(&NewDataReady);

  float distance = -1.0;

  if ((!status) && (NewDataReady != 0))
  {
    sensor_vl53l8cx_top.vl53l8cx_get_ranging_data(&Results);
    float min = 10000.0;
    for (int i = 0; i < VL53L8CX_RESOLUTION_8X8 * VL53L8CX_NB_TARGET_PER_ZONE; i++)
    {
      if (Results.target_status[i] != 255)
      {
        float distance = Results.distance_mm[i];
        if (min > distance)
        {
          min = distance;
        }
      }
    }
    distance = (min == 10000.0) ? 0.0 : min / 10.0;
    Serial.printf("Distance: %.2f cm\n", distance);
  }

  if (measurementCallback)
  {
    measurementCallback({distance});
  }

  if (sendBLE)
  {
    notifyBLE(distance);
  }
}

void DistanceSensor::notifyBLE(float distance)
{
  BLEModule::writeBLE(distanceCharacteristic, distance);
}
