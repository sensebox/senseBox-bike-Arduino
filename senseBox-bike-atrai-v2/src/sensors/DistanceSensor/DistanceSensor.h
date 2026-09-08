#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include "../BaseSensor.h"
#include <Wire.h>
#include <vl53l8cx.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


class DistanceSensor : public BaseSensor
{
public:
  DistanceSensor();
  bool readSensorData() override;
  static bool isPresent();

protected:
  void initSensor() override;
  void notifyBLE(float distance, float overtakingPredictionPercentage, float bikeOvertakingPredictionPercentage);
};

#endif // DISTANCESENSOR_H