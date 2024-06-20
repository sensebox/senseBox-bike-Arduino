#include "SoundSensor.h"

SoundSensor::SoundSensor() : BaseSensor("soundSensorTask", 2048, 1000) {}

// "b0221eb3-60f9-4842-9086-62ebbffd3c6e";
String soundUUID = "B0221EB360F94842908662EBBFFD3C6E";
int soundCharacteristic = 0;

const int soundPin = A5;

void SoundSensor::initSensor()
{
  // init sensor

  soundCharacteristic = BLEModule::createCharacteristic(soundUUID.c_str());
  // add more if needed
}

void SoundSensor::readSensorData()
{
  float soundValueValue = analogRead(soundPin);

  if (measurementCallback)
  {
    measurementCallback({soundValueValue});
  }

  if (sendBLE)
  {
    notifyBLE(soundValueValue);
  }
}

void SoundSensor::notifyBLE(float loudness)
{
  BLEModule::writeBLE(soundCharacteristic, loudness);
}