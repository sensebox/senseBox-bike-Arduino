#include "BatterySensor.h"

BatterySensor::BatterySensor() : BaseSensor("batterySensorTask", 2048, 10000) {}

// 5b262dea-4565-4ea0-912f-1e453bda0ca7
// String batteryUUID = "5B262DEA45654EA0912F1E453BDA0CA7";
String batteryUUID = "2A19";
int batteryCharacteristic = 0;

Adafruit_MAX17048 maxlipo;

// add more if needed

void BatterySensor::initSensor()
{
  while (!maxlipo.begin())
  {
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    delay(2000);
  }

  BLEModule::createService("180F");
  batteryCharacteristic = BLEModule::createCharacteristic(batteryUUID.c_str());
  // add more if needed
}

void BatterySensor::readSensorData()
{
  // read sensor data
  float batteryCharge = maxlipo.cellPercent();

  if (measurementCallback)
  {
    measurementCallback({batteryCharge});
  }

  if (sendBLE)
  {
    notifyBLE(batteryCharge);
  }
}

void BatterySensor::notifyBLE(float batteryCharge)
{
  BLEModule::writeBLE(batteryCharacteristic, batteryCharge);
}

float BatterySensor::getBatteryCharge()
{
  float batteryCharge = maxlipo.cellPercent();
  return batteryCharge;
}