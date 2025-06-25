#include "TempHumiditySensor.h"

TempHumiditySensor::TempHumiditySensor() : BaseSensor("temperatureHumidityTask", 
4096, // taskStackSize
970, // taskDelay
1, // taskPriority
1 // core
) {}

String tempUUID = "2cdf2174-35be-fdc4-4Ca2-6fd173f8b3a8";
String humUUID = "772df7ec-8cdc-4ea9-86af-410abe0ba257";

int temperatureCharacteristic = 0;
int humidityCharacteristic = 0;

Adafruit_HDC1000 hdc;

void TempHumiditySensor::initSensor()
{
  if (!hdc.begin())
  {
    Serial.println("Couldn't find HDC1080 sensor!");
    while (1)
      ;
  }

  temperatureCharacteristic = BLEModule::createCharacteristic(tempUUID.c_str());
  humidityCharacteristic = BLEModule::createCharacteristic(humUUID.c_str());
}
unsigned long startTemTime = millis();
bool TempHumiditySensor::readSensorData()
{
  float temperature = hdc.readTemperature();
  float humidity = hdc.readHumidity();

  if (temperature < -37 || humidity < 2)
  {
    Serial.println("Invalid temperature or humidity value");
    return false;
  }

  if (measurementCallback)
  {
    measurementCallback({temperature, humidity});
  }

  unsigned long endTemTime = millis();
  Serial.printf("temperature/humidity: %lu ms\n", endTemTime - startTemTime);
  startTemTime = millis();
  if (sendBLE)
  {
    notifyBLE(temperature, humidity);
  }
  return false;
}

void TempHumiditySensor::notifyBLE(float temperature, float humidity)
{
  BLEModule::writeBLE(tempUUID.c_str(), temperature);
  BLEModule::writeBLE(humUUID.c_str(), humidity);
}