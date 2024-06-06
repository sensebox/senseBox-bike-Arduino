#ifndef SAMPLE_SENSOR_H
#define SAMPLE_SENSOR_H

#include "../BaseSensor.h"

// include necceary libraries

class SampleSensor : public BaseSensor
{
public:
  SampleSensor();

protected:
  void initSensor() override;
  void readSensorData() override;
  void notifyBLE(float sampleValue); // change this to match the data type of the sensor data
};

#endif // SAMPLE_SENSOR_H
