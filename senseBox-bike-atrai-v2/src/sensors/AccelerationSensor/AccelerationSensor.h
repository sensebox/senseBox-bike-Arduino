#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MPU6050.h>

class AccelerationSensor : public BaseSensor
{
public:
  AccelerationSensor();

protected:
  void initSensor() override;
  void readSensorData() override;
  void notifyBLE(float x, float y, float z);
};

#endif // ACCELERATION_SENSOR_H
