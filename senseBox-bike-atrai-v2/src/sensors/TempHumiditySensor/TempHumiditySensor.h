#ifndef TEMP_HUMIDITY_SENSOR_H
#define TEMP_HUMIDITY_SENSOR_H

#include "../BaseSensor.h"
#include "Adafruit_HDC1000.h"

class TempHumiditySensor : public BaseSensor
{
public:
  TempHumiditySensor();

protected:
  void initSensor() override;
  void readSensorData() override;
  void notifyBLE(float temperature, float humidity);
};

#endif // DISTANCESENSOR_H
