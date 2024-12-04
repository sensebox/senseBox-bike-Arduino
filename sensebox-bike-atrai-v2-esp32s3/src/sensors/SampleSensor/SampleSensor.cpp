#include "SampleSensor.h"

SampleSensor::SampleSensor() : BaseSensor("sampleSensorTask", 
2048, // taskStackSize
1000, // taskDelay
1, // taskPriority
1 // core
) {}

String sampleUUID = "00000000000000000";
int sampleCharacteristic = 0;

// add more if needed

void SampleSensor::initSensor()
{
  // init sensor

  // sampleCharacteristic = BLEModule::createCharacteristic(sampleUUID.c_str());
  // add more if needed
}

bool SampleSensor::readSensorData()
{
  float sampleValue = 0.0;
  // read sensor data

  if (measurementCallback)
  {
    measurementCallback({sampleValue});
  }

  if (sendBLE)
  {
    notifyBLE(sampleValue);
  }
  return false;
}

void SampleSensor::notifyBLE(float sampleValue)
{
  // BLEModule::writeBLE(sampleCharacteristic, sampleValue);
}