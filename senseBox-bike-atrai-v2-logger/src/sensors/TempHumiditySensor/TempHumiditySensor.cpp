#include "TempHumiditySensor.h"

TempHumiditySensor::TempHumiditySensor() : BaseSensor("temperatureHumidityTask", 2048, 1000) {}

String tempUUID = "2cdf2174-35be-fdc4-4ca2-6fd173f8b3a8";
String humUUID = "772df7ec-8cdc-4ea9-86af-410abe0ba257";
String distanceUUID = "b3491b60-c0f3-4306-a30d-49c91f37a62b";
String overtakingUUID = "fc01c688-2c44-4965-ae18-373af9fed18d";

int temperatureCharacteristic = 0;
int humidityCharacteristic = 0;

Adafruit_HDC1000 hdc;

bool TempHumiditySensor::initSensor()
{
    if (!hdc.begin())
    {
        Serial.println("Couldn't find HDC1080 sensor!");
        return false;
    }

    temperatureCharacteristic = BLEModule::createCharacteristic(tempUUID.c_str());
    humidityCharacteristic = BLEModule::createCharacteristic(humUUID.c_str());

    return true;
}

bool TempHumiditySensor::readSensorData()
{
    float temperature = hdc.readTemperature();
    float humidity = hdc.readHumidity();

    if (isnan(temperature) ||
        isnan(humidity) ||
        temperature < -37 ||
        temperature > 125 ||
        humidity < 0 ||
        humidity > 100)
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
  BLEModule::writeBLE(tempUUID.c_str(), temperature);
  BLEModule::writeBLE(humUUID.c_str(), humidity);
}