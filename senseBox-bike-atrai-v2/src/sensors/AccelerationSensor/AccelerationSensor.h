#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_ICM20948.h> // Include for ICM20948

class AccelerationSensor : public BaseSensor
{
public:
  AccelerationSensor();
  bool readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float probAsphalt, float probCompact, float probPaving, float probSett, float probStanding, float anomaly);

private:
  enum SensorType
  {
    NONE,
    MPU6050,
    ICM20948
  };

  SensorType activeSensor = NONE; // Track which sensor is active
  Adafruit_MPU6050 mpu;
  Adafruit_ICM20948 icm;
};

#endif // ACCELERATION_SENSOR_H