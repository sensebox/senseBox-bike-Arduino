#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MPU6050.h>
#include "ICM42670P.h" // Include for ICM42670P

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
    ICM42670X // ICM42670X is a placeholder for the ICM42670P
  };

  SensorType activeSensor = NONE; // Track which sensor is active
  Adafruit_MPU6050 mpu;
  ICM42670 icm = ICM42670(Wire1, 0); // Instantiate an ICM42670 with LSB address set to 0
};

#endif // ACCELERATION_SENSOR_H