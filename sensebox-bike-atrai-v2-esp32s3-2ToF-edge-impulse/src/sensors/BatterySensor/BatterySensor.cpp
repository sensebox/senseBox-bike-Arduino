#include "BatterySensor.h"

BatterySensor::BatterySensor() : BaseSensor("batterySensorTask", 
4096, // taskStackSize, 
970, // taskDelay,
1, // taskPriority,
1 // core
) {}

String batteryUUID = "5b262dea-4565-4ea0-912f-1e453bda0ca7";
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
  // BLEModule::createService("180F");
  batteryCharacteristic = BLEModule::createCharacteristic(batteryUUID.c_str());
  // add more if needed
}

unsigned long startBatTime = millis();
bool BatterySensor::readSensorData()
{
  // read sensor data
  float batteryCharge = maxlipo.cellPercent();

  if (measurementCallback)
  {
    measurementCallback({batteryCharge});
  }

  unsigned long endBatTime = millis();
  Serial.printf("battery: %lu ms\n", endBatTime - startBatTime);
  startBatTime = millis();
  if (sendBLE)
  {
    notifyBLE(batteryCharge);
  }
  return false;
}

void BatterySensor::notifyBLE(float batteryCharge)
{
  BLEModule::writeBLE(batteryUUID.c_str(), batteryCharge);
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