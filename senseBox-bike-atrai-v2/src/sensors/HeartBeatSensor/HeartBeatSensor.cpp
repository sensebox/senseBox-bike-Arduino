#include "HeartBeatSensor.h"

HeartBeatSensor::HeartBeatSensor() : BaseSensor("heartBeatSensorTask", 2048, 1000) {}

// String heartBeatUUID = "e7110fe4-21a4-49e8-9b9a-baebd3427e81";
String heartBeatUUID = "E7110FE421A449E89B9ABAEBD3427E81";
int heartBeatCharacteristic = 0;

const int heartPin = A6;

void HeartBeatSensor::initSensor()
{
  // init sensor

  heartBeatCharacteristic = BLEModule::createCharacteristic(heartBeatUUID.c_str());
  // add more if needed
}

void HeartBeatSensor::readSensorData()
{
  float heartValue = analogRead(heartPin);

  Serial.println(heartValue);

  if (measurementCallback)
  {
    measurementCallback({heartValue});
  }

  if (sendBLE)
  {
    notifyBLE(heartValue);
  }
}

void HeartBeatSensor::notifyBLE(float bpm)
{
  BLEModule::writeBLE(heartBeatCharacteristic, bpm);
}