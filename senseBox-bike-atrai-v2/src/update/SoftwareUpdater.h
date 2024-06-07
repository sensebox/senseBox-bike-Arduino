#ifndef SOFTWARE_UPDATER_H
#define SOFTWARE_UPDATER_H

#include <Arduino.h>
#include <Arduino_ESP32_OTA.h>

#include <WiFi.h>


static char const OTA_FILE_LOCATION[] = "https://raw.githubusercontent.com/felixerdy/esp32-ota-test/main/senseBox-bike-atrai.ino.ota";

class SoftwareUpdater
{
public:
    SoftwareUpdater();

    void setSSID(String ssid);
    void setPassword(String password);

    void startUpdate();

private:
    String ssid;
    String password;
};

#endif // SOFTWARE_UPDATER_H
