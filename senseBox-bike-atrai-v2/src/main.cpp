#include <Arduino.h>
#include "sensors/TempHumiditySensor/TempHumiditySensor.h"
#include "sensors/DustSensor/DustSensor.h"
#include "sensors/DistanceSensor/DistanceSensor.h"
#include "sensors/AccelerationSensor/AccelerationSensor.h"
#include "sensors/HeartBeatSensor/HeartBeatSensor.h"
#include "sensors/SoundSensor/SoundSensor.h"

#include "display/Display.h"
#include "ble/BLEModule.h"
#include "led/LED.h"

DustSensor dustSensor;
TempHumiditySensor tempHumiditySensor;
DistanceSensor distanceSensor;
AccelerationSensor accelerationSensor;
HeartBeatSensor heartBeatSensor;
SoundSensor soundSensor;

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

    SBDisplay::showLoading("Distance...", 0.2);
    distanceSensor.begin();

    SBDisplay::showLoading("Dust...", 0.3);
    dustSensor.begin();

    SBDisplay::showLoading("Acceleration...", 0.4);
    accelerationSensor.begin();

    SBDisplay::showLoading("Temperature...", 0.5);
    tempHumiditySensor.begin();

    SBDisplay::showLoading("Heart Beat...", 0.55);
    heartBeatSensor.begin();

    SBDisplay::showLoading("Sound...", 0.56);
    soundSensor.begin();

    SBDisplay::showLoading("Ventilation...", 0.6);
    pinMode(3, OUTPUT);
    delay(100);
    digitalWrite(3, HIGH);

    SBDisplay::showLoading("Start measurements...", 1);

    dustSensor.startSubscription();
    tempHumiditySensor.startSubscription();
    distanceSensor.startSubscription();
    accelerationSensor.startSubscription();
    heartBeatSensor.startSubscription();
    soundSensor.startSubscription();

    dustSensor.startBLE();
    tempHumiditySensor.startBLE();
    distanceSensor.startBLE();
    accelerationSensor.startBLE();
    heartBeatSensor.startBLE();
    soundSensor.startBLE();

    display.showConnectionScreen();

    led.stopRainbow();
}

void loop()
{
    dustSensor.readSensorData();
    tempHumiditySensor.readSensorData();
    accelerationSensor.readSensorData();
    distanceSensor.readSensorData();
    heartBeatSensor.readSensorData();
    soundSensor.readSensorData();

    bleModule.blePoll();
}
