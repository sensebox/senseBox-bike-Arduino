#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include "../BaseSensor.h"
#include <Wire.h>
#include <vl53l8cx.h>

class DistanceSensor : public BaseSensor
{
public:
  DistanceSensor();
  bool readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float distance, float overtakingPredictionPercentage);
};

#endif // DISTANCESENSOR_H
