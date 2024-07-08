#ifndef TEMP_HUMIDITY_SENSOR_H
#define TEMP_HUMIDITY_SENSOR_H

#include "../BaseSensor.h"
#include "Adafruit_HDC1000.h"

class TempHumiditySensor : public BaseSensor
{
public:
  TempHumiditySensor();
  void readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float temperature, float humidity);
};

#endif // TEMP_HUMIDITY_SENSOR_H
