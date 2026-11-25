#include "BatterySensor.h"
#include <display/Display.h>

BatterySensor::BatterySensor() : BaseSensor("batterySensorTask", 2048, 10000) {}

// 5b262dea-4565-4ea0-912f-1e453bda0ca7
// String batteryUUID = "5B262DEA45654EA0912F1E453BDA0CA7";
String batteryUUID = "2A19";
int batteryCharacteristic = 0;

Adafruit_MAX17048 maxlipo;

void BatterySensor::initSensor()
{
  maxlipo.begin();
  delay(100);
  tcaselect(2);
  delay(100);
  while (!maxlipo.isDeviceReady())
  {
    SBDisplay::showLoading("MAX17048 Error",  0);
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    delay(2000);
  }

  BLEModule::createService("180F");
  batteryCharacteristic = BLEModule::createCharacteristic(batteryUUID.c_str());
  // add more if needed
}

bool BatterySensor::readSensorData()
{
  tcaselect(2);
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
  return false;
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

float BatterySensor::getBatteryChargeRate()
{
  float batteryChargeRate = maxlipo.chargeRate();
  return batteryChargeRate;
}