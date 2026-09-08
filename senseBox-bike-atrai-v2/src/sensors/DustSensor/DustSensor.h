#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "../BaseSensor.h"
#include <sps30.h>
#include <Wire.h>

// include necceary libraries

class DustSensor : public BaseSensor
{
public:
  DustSensor();
  bool readSensorData() override;
  static bool isPresent();

protected:
  void initSensor() override;
  void notifyBLE(float pm1, float pm2_5, float pm4, float pm10);

  static const int MAX_INIT_ATTEMPTS = 7;
};

#endif // DUST_SENSOR_H
