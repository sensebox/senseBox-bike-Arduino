#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "../BaseSensor.h"
#include <sps30.h>

// include necceary libraries

class DustSensor : public BaseSensor
{
public:
  DustSensor();
  void readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float pm1, float pm2_5, float pm4, float pm10);
};

#endif // DUST_SENSOR_H
