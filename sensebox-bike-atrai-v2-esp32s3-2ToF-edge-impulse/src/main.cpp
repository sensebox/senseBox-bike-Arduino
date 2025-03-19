#include "globals.h"
SemaphoreHandle_t i2c_mutex;

#include <Arduino.h>
#include "sensors/TempHumiditySensor/TempHumiditySensor.h"
// #include "sensors/DustSensor/DustSensor.h"

#include "AccelerationDistanceSensor/AccelerationDistanceSensor.h"
#include "sensors/BatterySensor/BatterySensor.h"
#include "display/Display.h"
#include "ble/BLEModule.h"
// #include "led/LED.h"

// DustSensor dustSensor;
TempHumiditySensor tempHumiditySensor;
AccelerationDistanceSensor accelerationDistanceSensor;
BatterySensor batterySensor;

BaseSensor *sensors[] = {
    // &dustSensor,
    &batterySensor,
    &tempHumiditySensor,
    };

SBDisplay display;

BLEModule bleModule;
// LED led(1, 1);

unsigned long previousMillis = 0; // stores the last time the sensors were read
const long interval = 3000;       // interval at which to read the temperature and fine dust sensors (1 second)

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // led.begin();

    // led.startRainbow();

    i2c_mutex = xSemaphoreCreateMutex();
    if (i2c_mutex == NULL) {
        // Handle error: Mutex creation failed
    }

    SBDisplay::begin();

    // pinMode(IO_ENABLE, OUTPUT);
    // digitalWrite(IO_ENABLE, LOW);

    SBDisplay::showLoading("Setup BLE...", 0.2);
    bleModule.begin();

    bleModule.createService("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

    // batterySensor.begin();

    SBDisplay::showLoading("Setup Sensors...", 0.4);
    for (BaseSensor *sensor : sensors)
    {
        sensor->begin();
    }
    accelerationDistanceSensor.begin();
    Serial.println("Setup complete");

    SBDisplay::showLoading("Ventilation...", 0.6);
    // pinMode(3, OUTPUT);
    // delay(100);
    // digitalWrite(3, HIGH);

    SBDisplay::showLoading("Start measurements...", 0.8);
    // Start sensor measurements
    for (BaseSensor *sensor : sensors)
    {
        sensor->startSubscription();
    }
    accelerationDistanceSensor.startSubscription();
    Serial.println("Subscriptions started");

    SBDisplay::showLoading("Enable BLE...", 1);

    // Start BLE advertising
    for (BaseSensor *sensor : sensors)
    {
        sensor->startBLE();
    }
    accelerationDistanceSensor.startBLE();
    Serial.println("BLE enabled");

    display.readBleId();

    // led.stopRainbow();

    const char* macString = bleModule.getMacAddress();
    String bleId = "[" + String(macString) + "]";
    String bleIdBegin = bleId.substring(0, bleId.length() / 2);
    String bleIdEnd = bleId.substring(bleId.length() / 2);
    String name = "senseBox:bike " + bleId;
    const char *message[] = {
      "senseBox",
      "bike",
      bleIdBegin.c_str(),
      bleIdEnd.c_str()
    };

    display.showConnectionScreen(name, message);
    bleModule.bleStartPoll("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
}

void loop()
{
    // Read acceleration and distance sensor data as fast as possible
    // distanceSensor.readSensorData();
    // bool classified = accelerationSensor.readSensorData();

    // Read temperature and fine dust sensor data after a surface classification
    // if (classified)
    // {
    //     dustSensor.readSensorData();
    //     tempHumiditySensor.readSensorData();
    //     display.showConnectionScreen();
    // }

    // Perform BLE polling
}
