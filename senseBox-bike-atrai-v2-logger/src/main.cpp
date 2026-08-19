#include <Arduino.h>
#include "sensors/TempHumiditySensor/TempHumiditySensor.h"
#include "sensors/BatterySensor/BatterySensor.h"
#include "display/Display.h"
#include "ble/BLEModule.h"
#include "led/LED.h"

TempHumiditySensor tempHumiditySensor;
BatterySensor batterySensor;

BaseSensor *sensors[] = {
    &tempHumiditySensor,
    &batterySensor};

SBDisplay display;

BLEModule bleModule;
//LED led(1, 45);
LED led(PIN_NEOPIXEL, 1, 45);

unsigned long previousMillis = 0; // stores the last time the sensors were read
const long interval = 1000;       // interval at which to read the temperature and humidity sensors (1 second)

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("FEATHER BOOT OK");
    //Wire.begin(2,1); //senseBox MCU eye
    Wire.begin(); // adafruit feather esp32-s3
    delay(1000);



     //led.begin();

     //led.startRainbow();

    // SBDisplay::begin();

    // SBDisplay::showLoading("Setup BLE...", 0.2);
    bleModule.begin();
    bleModule.createService("CF06A218-F68E-E0BE-AD04-8EBC1EB0BC84");
    Serial.println("BLE setup complete");

    batterySensor.begin();

    // SBDisplay::showLoading("Setup Sensors...", 0.4);
    for (BaseSensor *sensor : sensors)
    {
        sensor->begin();
    }

    // SBDisplay::showLoading("Ventilation...", 0.6);

    //senseBox MCU eye
    //pinMode(48, OUTPUT);
    //delay(100);
    //digitalWrite(48, HIGH);

    // adafruit feather esp32-s3
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, NEOPIXEL_POWER_ON);


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
    // display.showConnectionScreen(name, message);

    // SBDisplay::showLoading("Start measurements...", 0.8);
    // Start sensor measurements
    for (BaseSensor *sensor : sensors)
    {
        sensor->startSubscription();
    }

    // SBDisplay::showLoading("Enable BLE...", 1);

    // Start BLE advertising
    for (BaseSensor *sensor : sensors)
    {
        sensor->startBLE();
    }
    bleModule.startService();
    Serial.println("BLE enabled");

    // display.readBleId();

    // led.stopRainbow();

    bleModule.bleStartPoll("CF06A218-F68E-E0BE-AD04-8EBC1EB0BC84");

    // display.showConnectionScreen();
}

void loop()
{

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        tempHumiditySensor.readSensorData();
    }
}
