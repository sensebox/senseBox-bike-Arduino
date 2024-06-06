#include <Arduino.h>
#include "sensors/TempHumiditySensor/TempHumiditySensor.h"
#include "sensors/DustSensor/DustSensor.h"
#include "sensors/DistanceSensor/DistanceSensor.h"
#include "sensors/AccelerationSensor/AccelerationSensor.h"
#include "display/Display.h"
#include "ble/BLEModule.h"

#include "Adafruit_NeoPixel.h"

DustSensor dustSensor;
TempHumiditySensor tempHumiditySensor;
DistanceSensor distanceSensor;
AccelerationSensor accelerationSensor;

SBDisplay display;

BLEModule bleModule;

Adafruit_NeoPixel rgb_led = Adafruit_NeoPixel(1, 1, NEO_GRB + NEO_KHZ800);

void setup()
{
    Serial.begin(115200);
    delay(1000);

    rgb_led.begin();

    rgb_led.setBrightness(30);

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

    // Subscribe to sensor measurements
    // tempHumiditySensor.subscribe([](String uuid, std::vector<float> values) {
    // float temperature = values[0];
    // float humidity = values[1];
    // Serial.printf("TempHumiditySensor [%s]: %.2f, %.2f\n", uuid.c_str(), temperature, humidity); });

    //   dustSensor.subscribe([](String uuid) {
    //     Serial.printf("DustSensor [%s]\n", uuid.c_str());
    //   }, "UUID-Dust");

    // distanceSensor.subscribe([](std::vector<float> values)
    //                          { Serial.printf("DistanceSensor [%s]: %.2f\n", values[0]); });

    // accelerationSensor.subscribe([](String uuid, std::vector<float> values)
    //                              {
    //                             float x = values[0];
    //                             float y = values[1];
    //                             float z = values[2];
    //                             Serial.printf("Acceleration [%s]: %.2f, %.2f, %.2f\n", uuid.c_str(), x, y, z); });

    dustSensor.startSubscription();
    tempHumiditySensor.startSubscription();
    distanceSensor.startSubscription();
    accelerationSensor.startSubscription();

    dustSensor.startBLE();
    tempHumiditySensor.startBLE();
    distanceSensor.startBLE();
    accelerationSensor.startBLE();

    display.showConnectionScreen();
}

void loop()
{
}
