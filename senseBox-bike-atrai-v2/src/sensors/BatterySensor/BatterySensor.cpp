#include "BatterySensor.h"
#include <display/Display.h>

BatterySensor::BatterySensor() : BaseSensor("batterySensorTask", 2048, 10000) {}

// 5b262dea-4565-4ea0-912f-1e453bda0ca7
// String batteryUUID = "5B262DEA45654EA0912F1E453BDA0CA7";
String batteryUUID = "2A19";
int batteryCharacteristic = 0;

Adafruit_MAX17048 maxlipo;
static bool sensorFound = false;

// add more if needed

void BatterySensor::initSensor()
{
  for (int attempt = 1; attempt <= MAX_INIT_ATTEMPTS && !sensorFound; attempt++)
  {
    if (maxlipo.begin())
    {
      sensorFound = true;
      break;
    }
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    delay(1000);
  }

  if (!sensorFound)
  {
    SBDisplay::showLoadingError("No Battery Sensor");
    Serial.printf("MAX17048 not found after %d attempts, continuing without battery sensor.\n", MAX_INIT_ATTEMPTS);
    delay(2000);
    return;
  }

  BLEModule::createService("180F");
  batteryCharacteristic = BLEModule::createCharacteristic(batteryUUID.c_str());
}

bool BatterySensor::readSensorData()
{
  if (!sensorFound)
  {
    return false;
  }

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

bool BatterySensor::isPresent()
{
  return sensorFound;
}

float BatterySensor::getBatteryCharge()
{
  if (!sensorFound)
  {
    return 0;
  }
  float batteryCharge = maxlipo.cellPercent();
  return batteryCharge;
}

float BatterySensor::getBatteryChargeRate()
{
  if (!sensorFound)
  {
    return 0;
  }
  float batteryChargeRate = maxlipo.chargeRate();
  return batteryChargeRate;
}