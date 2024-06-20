#ifndef HEART_BEAT_SENSOR_H
#define HEART_BEAT_SENSOR_H

#include "../BaseSensor.h"

// include necceary libraries

class HeartBeatSensor : public BaseSensor
{
public:
  HeartBeatSensor();
  void readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float bpm); // change this to match the data type of the sensor data
};

#endif // HEART_BEAT_SENSOR_H
