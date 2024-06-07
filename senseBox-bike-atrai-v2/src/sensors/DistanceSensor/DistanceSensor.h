#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include "../BaseSensor.h"
#include <Wire.h>
#include <vl53l8cx_class.h>

class DistanceSensor : public BaseSensor
{
public:
  DistanceSensor();

protected:
  void initSensor() override;
  void readSensorData() override;
  void notifyBLE(float distance);
};

#endif // DISTANCESENSOR_H
