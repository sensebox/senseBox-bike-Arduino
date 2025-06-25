#ifndef SAMPLE_SENSOR_H
#define SAMPLE_SENSOR_H

#include "../BaseSensor.h"

// include necessary libraries

class SampleSensor : public BaseSensor
{
public:
  SampleSensor();

protected:
  void initSensor() override;
  bool readSensorData() override;
  void notifyBLE(float sampleValue); // change this to match the data type of the sensor data
};

#endif // SAMPLE_SENSOR_H
