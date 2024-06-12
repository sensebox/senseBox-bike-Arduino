#ifndef SAMPLE_SENSOR_H
#define SAMPLE_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MAX1704X.h>

// include necceary libraries

class BatterySensor : public BaseSensor
{
public:
  BatterySensor();
  static float getBatteryCharge();

protected:
  void initSensor() override;
  void readSensorData() override;
  void notifyBLE(float batteryCharge); // change this to match the data type of the sensor data
};

#endif // SAMPLE_SENSOR_H
