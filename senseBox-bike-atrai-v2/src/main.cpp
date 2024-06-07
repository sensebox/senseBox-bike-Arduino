#include <Arduino.h>
#include "sensors/TempHumiditySensor/TempHumiditySensor.h"
#include "sensors/DustSensor/DustSensor.h"
#include "sensors/DistanceSensor/DistanceSensor.h"
#include "sensors/AccelerationSensor/AccelerationSensor.h"
#include "display/Display.h"
#include "ble/BLEModule.h"
#include "update/SoftwareUpdater.h"

// --- Sensor instances ---
DustSensor dustSensor;
TempHumiditySensor tempHumiditySensor;
DistanceSensor distanceSensor;
AccelerationSensor accelerationSensor;

BaseSensor *sensors[] = {&dustSensor, &tempHumiditySensor, &distanceSensor, &accelerationSensor};

// --- Display instance ---
SBDisplay display;

// --- BLE instance ---
BLEModule bleModule;

// --- SoftwareUpdater instance ---
SoftwareUpdater softwareUpdater;

// create an enum with the different states and modes the device can be in
enum class State
{
    IDLE,
    MEASURING,
    CONNECTED,
    WIFI_READ_SSID,
    WIFI_READ_PASSWORD,
    START_SOFTWARE_UPDATE
};

void setup()
{
    Serial.begin(115200);
    delay(1000);

    SBDisplay::begin();

    pinMode(IO_ENABLE, OUTPUT);
    digitalWrite(IO_ENABLE, LOW);

    SBDisplay::showLoading("Setup BLE...", 0.1);
    bleModule.begin();

    bleModule.receiveCallback = [](String data)
    {
        // get first character of the string and parse it to int
        int mode = data.charAt(0) - '0';

        // payload is all behind the first character
        String payload = data.substring(1);

        // mode to state
        State state = static_cast<State>(mode);

        Serial.printf("Received mode: %d\n", mode);

        // switch case with mode and the state enum
        switch (state)
        {
        case State::IDLE:
            SBDisplay::showLoading("Idle...", 0.1);
            for (BaseSensor *sensor : sensors)
            {
                sensor->stopBLE();
            }
            for (BaseSensor *sensor : sensors)
            {
                sensor->stopSubscription();
            }
            break;
        case State::MEASURING:
            SBDisplay::showLoading("Measuring...", 0.1);
            for (BaseSensor *sensor : sensors)
            {
                sensor->startSubscription();
            }
            break;
        case State::CONNECTED:
            SBDisplay::showLoading("Connected...", 0.1);
            for (BaseSensor *sensor : sensors)
            {
                sensor->startBLE();
            }
            break;
        case State::WIFI_READ_SSID:
            SBDisplay::showLoading(payload, 0.1);
            softwareUpdater.setSSID(payload);
            break;
        case State::WIFI_READ_PASSWORD:
            SBDisplay::showLoading(payload, 0.1);
            softwareUpdater.setPassword(payload);
            break;
        case State::START_SOFTWARE_UPDATE:
            SBDisplay::showLoading("Updating...", 0.1);
            softwareUpdater.startUpdate();
            break;
        default:
            break;
        }
    };

    // SBDisplay::showLoading("Distance...", 0.2);
    // distanceSensor.begin();

    // SBDisplay::showLoading("Dust...", 0.3);
    // dustSensor.begin();

    // SBDisplay::showLoading("Acceleration...", 0.4);
    // accelerationSensor.begin();

    // SBDisplay::showLoading("Temperature...", 0.5);
    // tempHumiditySensor.begin();

    // SBDisplay::showLoading("Ventilation...", 0.6);
    // pinMode(3, OUTPUT);
    // delay(100);
    // digitalWrite(3, HIGH);

    // SBDisplay::showLoading("Start measurements...", 1);

    // // Subscribe to sensor measurements
    // // tempHumiditySensor.subscribe([](String uuid, std::vector<float> values) {
    // // float temperature = values[0];
    // // float humidity = values[1];
    // // Serial.printf("TempHumiditySensor [%s]: %.2f, %.2f\n", uuid.c_str(), temperature, humidity); });

    // //   dustSensor.subscribe([](String uuid) {
    // //     Serial.printf("DustSensor [%s]\n", uuid.c_str());
    // //   }, "UUID-Dust");

    // // distanceSensor.subscribe([](std::vector<float> values)
    // //                          { Serial.printf("DistanceSensor [%s]: %.2f\n", values[0]); });

    // // accelerationSensor.subscribe([](String uuid, std::vector<float> values)
    // //                              {
    // //                             float x = values[0];
    // //                             float y = values[1];
    // //                             float z = values[2];
    // //                             Serial.printf("Acceleration [%s]: %.2f, %.2f, %.2f\n", uuid.c_str(), x, y, z); });

    display.showConnectionScreen();
}

void loop()
{
}
