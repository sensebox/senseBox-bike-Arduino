#include "TempHumiditySensor.h"
#include <display/Display.h>

TempHumiditySensor::TempHumiditySensor() : BaseSensor("temperatureHumidityTask", 2048, 1000) {}

String tempUUID = "2CDF217435BEFDC44CA26FD173F8B3A8";
String humUUID = "772DF7EC8CDC4EA986AF410ABE0BA257";

int temperatureCharacteristic = 0;
int humidityCharacteristic = 0;

Adafruit_HDC1000 hdc;
bool tempHumiditySensorFound = false;

void TempHumiditySensor::initSensor()
{
  for (int attempt = 1; attempt <= MAX_INIT_ATTEMPTS && !tempHumiditySensorFound; attempt++)
  {
    if (hdc.begin())
    {
      tempHumiditySensorFound = true;
      break;
    }
    Serial.println("Couldn't find HDC1080 sensor!");
    delay(1000);
  }

  if (!tempHumiditySensorFound)
  {
    SBDisplay::showLoadingError("No Temp/Humid Sensor");
    Serial.printf("HDC1080 not found after %d attempts, continuing without temp/humidity sensor.\n", MAX_INIT_ATTEMPTS);
    delay(2000);
    return;
  }

  temperatureCharacteristic = BLEModule::createCharacteristic(tempUUID.c_str());
  humidityCharacteristic = BLEModule::createCharacteristic(humUUID.c_str());
}

bool TempHumiditySensor::readSensorData()
{
  if (!tempHumiditySensorFound)
  {
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
  return false;
}

void TempHumiditySensor::notifyBLE(float temoperature, float humidity)
{
  BLEModule::writeBLE(temperatureCharacteristic, temoperature);
  BLEModule::writeBLE(humidityCharacteristic, humidity);
}

bool TempHumiditySensor::isPresent()
{
  return tempHumiditySensorFound;
}