#include "TempHumiditySensor.h"

TempHumiditySensor::TempHumiditySensor() : BaseSensor("temperatureHumidityTask", 2048, 1000) {}

String tempUUID = "2CDF217435BEFDC44CA26FD173F8B3A8";
String humUUID = "772DF7EC8CDC4EA986AF410ABE0BA257";

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

void TempHumiditySensor::readSensorData()
{
  float temperature = hdc.readTemperature();
  float humidity = hdc.readHumidity();

  if (measurementCallback)
  {
    measurementCallback({temperature, humidity});
  }

  if (sendBLE)
  {
    notifyBLE(temperature, humidity);
  }
}

void TempHumiditySensor::notifyBLE(float temoperature, float humidity)
{
  BLEModule::writeBLE(temperatureCharacteristic, temoperature);
  BLEModule::writeBLE(humidityCharacteristic, humidity);
}