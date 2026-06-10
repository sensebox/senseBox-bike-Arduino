#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "../BaseSensor.h"
#include <Adafruit_MPU6050.h>
#include "ICM42670P.h" // Include for ICM42670P
#include <Adafruit_ICM20948.h> // Include for ICM20948
#include <Adafruit_Sensor.h> // Include for ICM20948

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
    ICM42670X, // ICM42670X is a placeholder for the ICM42670P
    ICM20948
  };

  SensorType activeSensor = NONE; // Track which sensor is active
  Adafruit_MPU6050 mpu;
  ICM42670 icm = ICM42670(Wire1, 0); // Instantiate an ICM42670 with LSB address set to 0
  Adafruit_ICM20948 icm2;
};

#endif // ACCELERATION_SENSOR_H