#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MPU6050.h>

class AccelerationSensor : public BaseSensor
{
public:
  AccelerationSensor();
  void readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float x, float y, float z);
};

#endif // ACCELERATION_SENSOR_H
