#ifndef SAMPLE_SENSOR_H
#define SAMPLE_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MAX1704X.h>

// include necessary libraries

class BatterySensor : public BaseSensor
{
public:
  BatterySensor();
  static float getBatteryCharge();
  static float getBatteryChargeRate();

protected:
  void initSensor() override;
  bool readSensorData() override;
  void notifyBLE(float batteryCharge); // change this to match the data type of the sensor data
};

#endif // SAMPLE_SENSOR_H
