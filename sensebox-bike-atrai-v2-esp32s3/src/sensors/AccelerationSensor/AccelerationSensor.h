#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MPU6050.h>

class AccelerationSensor : public BaseSensor
{
public:
  AccelerationSensor();
  bool readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float probAsphalt, float probCompact, float probPaving, float probSett, float probStanding, float anomaly);
};

#endif // ACCELERATION_SENSOR_H
