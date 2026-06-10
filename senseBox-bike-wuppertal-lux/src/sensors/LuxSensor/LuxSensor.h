#ifndef LUX_SENSOR_H
#define LUX_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_LTR329_LTR303.h>
#include <Wire.h>

// include necessary libraries

class LuxSensor : public BaseSensor
{
public:
  LuxSensor();
  bool readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float lux);
};

#endif // LUX_SENSOR_H
