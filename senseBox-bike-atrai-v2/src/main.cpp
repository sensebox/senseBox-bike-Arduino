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

SBDisplay display;

BLEModule bleModule;
LED led(1, 1);

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

    batterySensor.begin();

    bleModule.createService("CF06A218F68EE0BEAD048EBC1EB0BC84");

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

    dustSensor.startSubscription();
    tempHumiditySensor.startSubscription();
    distanceSensor.startSubscription();
    accelerationSensor.startSubscription();
    batterySensor.startSubscription();

    dustSensor.startBLE();
    tempHumiditySensor.startBLE();
    distanceSensor.startBLE();
    accelerationSensor.startBLE();
    batterySensor.startBLE();

    display.showConnectionScreen();

    led.stopRainbow();
}

void loop()
{
    if (bleModule.isConnected())
    {
        display.showSystemStatus();
    }
    delay(1000);
}
