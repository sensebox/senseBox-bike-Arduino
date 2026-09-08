#include <Arduino.h>
#include "sensors/TempHumiditySensor/TempHumiditySensor.h"
#include "sensors/DustSensor/DustSensor.h"
#include "sensors/DistanceSensor/DistanceSensor.h"
#include "sensors/AccelerationSensor/AccelerationSensor.h"
#include "sensors/BatterySensor/BatterySensor.h"
#include "display/Display.h"
#include "ble/BLEModule.h"
#include "led/LED.h"

DustSensor dustSensor;
TempHumiditySensor tempHumiditySensor;
DistanceSensor distanceSensor;
AccelerationSensor accelerationSensor;
BatterySensor batterySensor;

BaseSensor *sensors[] = {
    &dustSensor,
    &tempHumiditySensor,
    &distanceSensor,
    &accelerationSensor,
    &batterySensor};

SBDisplay display;

BLEModule bleModule;
LED led(1, 1);

unsigned long previousMillis = 0;
const long interval = 3000;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    led.begin();

    led.startRainbow();

    SBDisplay::begin();

    pinMode(IO_ENABLE, OUTPUT);
    digitalWrite(IO_ENABLE, LOW);

    SBDisplay::showLoading("Setup BLE...", 0.2);
    bleModule.begin();

    bleModule.createService("CF06A218F68EE0BEAD048EBC1EB0BC84");

    SBDisplay::showLoading("Setup Sensors...", 0.4);
    for (BaseSensor *sensor : sensors)
    {
        sensor->begin();
    }

    SBDisplay::showLoading("Ventilation...", 0.6);
    pinMode(3, OUTPUT);
    delay(100);
    digitalWrite(3, HIGH);

    SBDisplay::showLoading("Start measurements...", 0.8);
    // Start sensor measurements
    for (BaseSensor *sensor : sensors)
    {
        sensor->startSubscription();
    }

    SBDisplay::showLoading("Enable BLE...", 1);

    // Start BLE advertising
    for (BaseSensor *sensor : sensors)
    {
        sensor->startBLE();
    }

    display.readBleId();

    led.stopRainbow();

    display.showConnectionScreen();
}

void loop()
{
    // Read acceleration and distance sensor data as fast as possible
    distanceSensor.readSensorData();
    bool classified = accelerationSensor.readSensorData();

    // Read temperature and fine dust sensor data after a surface classification
    if (accelerationSensor.isPresent())
    {
        if (classified)
        {
            dustSensor.readSensorData();
            tempHumiditySensor.readSensorData();
            display.showConnectionScreen();
        }
    } else
    {
        // If no acceleration sensor is present, read the other sensors at a fixed interval
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            dustSensor.readSensorData();
            tempHumiditySensor.readSensorData();
            display.showConnectionScreen();
        }
    }

    // Perform BLE polling
    bleModule.blePoll();
}
