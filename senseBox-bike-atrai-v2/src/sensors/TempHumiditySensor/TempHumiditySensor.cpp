#include "TempHumiditySensor.h"

TempHumiditySensor::TempHumiditySensor() : BaseSensor("temperatureHumidityTask", 2048, 1000) {
  name = "Temp/Humid Sensor";
}

String tempUUID = "2CDF217435BEFDC44CA26FD173F8B3A8";
String humUUID = "772DF7EC8CDC4EA986AF410ABE0BA257";

int temperatureCharacteristic = 0;
int humidityCharacteristic = 0;

Adafruit_HDC1000 hdc;

void TempHumiditySensor::initSensor()
{
  for (int i = 0; i < maxInitAttempts && !initialized; i++)
  {
    initialized = hdc.begin();
    delay(100);
  }
  if (!initialized)
  {
    return;
  }
  Serial.println("HDC1080 initialized successfully");
  temperatureCharacteristic = BLEModule::createCharacteristic(tempUUID.c_str());
  humidityCharacteristic = BLEModule::createCharacteristic(humUUID.c_str());
}

bool TempHumiditySensor::readSensorData()
{
  if(!initialized)
  {
    Serial.println("HDC1080 not initialized");
    return false;
  }
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

  if (sendBLE)
  {
    notifyBLE(temperature, humidity);
  }
  return true;
}

void TempHumiditySensor::notifyBLE(float temperature, float humidity)
{
  BLEModule::writeBLE(temperatureCharacteristic, temperature);
  BLEModule::writeBLE(humidityCharacteristic, humidity);
}