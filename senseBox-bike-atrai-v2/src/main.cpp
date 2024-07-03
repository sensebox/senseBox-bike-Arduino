#include <Arduino.h>
#include "sensors/TempHumiditySensor/TempHumiditySensor.h"
#include "sensors/DustSensor/DustSensor.h"
#include "sensors/DistanceSensor/DistanceSensor.h"
#include "sensors/AccelerationSensor/AccelerationSensor.h"
#include "display/Display.h"
#include "ble/BLEModule.h"
#include "led/LED.h"

DustSensor dustSensor;
TempHumiditySensor tempHumiditySensor;
DistanceSensor distanceSensor;
AccelerationSensor accelerationSensor;

SBDisplay display;

BLEModule bleModule;
LED led(1, 1);

unsigned long previousMillis = 0; // stores the last time the sensors were read
const long interval = 1000;       // interval at which to read the temperature and fine dust sensors (1 second)

void setup()
{
    Serial.begin(115200);
    delay(1000);

    led.begin();

    led.startRainbow();

    SBDisplay::begin();

    pinMode(IO_ENABLE, OUTPUT);
    digitalWrite(IO_ENABLE, LOW);

    SBDisplay::showLoading("Setup BLE...", 0.1);
    bleModule.begin();

    SBDisplay::showLoading("Distance...", 0.2);
    distanceSensor.begin();

    SBDisplay::showLoading("Dust...", 0.3);
    dustSensor.begin();

    SBDisplay::showLoading("Acceleration...", 0.4);
    accelerationSensor.begin();

    SBDisplay::showLoading("Temperature...", 0.5);
    tempHumiditySensor.begin();

    SBDisplay::showLoading("Ventilation...", 0.6);
    pinMode(3, OUTPUT);
    delay(100);
    digitalWrite(3, HIGH);

    SBDisplay::showLoading("Start measurements...", 1);

    // dustSensor.startSubscription();
    // tempHumiditySensor.startSubscription();
    // distanceSensor.startSubscription();
    // accelerationSensor.startSubscription();

    dustSensor.startBLE();
    tempHumiditySensor.startBLE();
    distanceSensor.startBLE();
    accelerationSensor.startBLE();

    display.showConnectionScreen();

    led.stopRainbow();
}

void loop()
{
    unsigned long currentMillis = millis();

    // Read temperature and fine dust sensor data at defined interval
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        dustSensor.readSensorData();
        tempHumiditySensor.readSensorData();
    }

    // Read acceleration and distance sensor data as fast as possible
    accelerationSensor.readSensorData();
    distanceSensor.readSensorData();

    // Perform BLE polling
    bleModule.blePoll();
}