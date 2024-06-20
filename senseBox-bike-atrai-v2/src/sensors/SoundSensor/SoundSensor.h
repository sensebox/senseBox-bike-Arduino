#ifndef SOUND_SENSOR_H
#define SOUND_SENSOR_H

#include "../BaseSensor.h"

// include necceary libraries

class SoundSensor : public BaseSensor
{
public:
  SoundSensor();
  void readSensorData() override;

protected:
  void initSensor() override;
  void notifyBLE(float loudness); // change this to match the data type of the sensor data
};

#endif // SOUND_SENSOR_H
