#include "SoftwareUpdater.h"
#include "root_ca.h"

SoftwareUpdater::SoftwareUpdater()
{
    ssid = "";
    password = "";
}

void SoftwareUpdater::setSSID(String ssid)
{
    this->ssid = ssid;
}

void SoftwareUpdater::setPassword(String password)
{
    this->password = password;
}

void SoftwareUpdater::startUpdate()
{
    // Check if the SSID and password are set
    if (ssid == "" || password == "")
    {
        return;
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to '");
        Serial.print(ssid);
        Serial.println("'");
        WiFi.begin(ssid, password);
        delay(2000);
    }
    Serial.print("You're connected to '");
    Serial.print(WiFi.SSID());
    Serial.println("'");

    Arduino_ESP32_OTA ota;
    Arduino_ESP32_OTA::Error ota_err = Arduino_ESP32_OTA::Error::None;

    /* Configure custom Root CA */
    ota.setCACert(root_ca);

    Serial.println("Initializing OTA storage");
    if ((ota_err = ota.begin()) != Arduino_ESP32_OTA::Error::None)
    {
        Serial.print("Arduino_ESP_OTA::begin() failed with error code ");
        Serial.println((int)ota_err);
        return;
    }

    Serial.println("Starting download to flash ...");
    int const ota_download = ota.download(OTA_FILE_LOCATION);
    if (ota_download <= 0)
    {
        Serial.print("Arduino_ESP_OTA::download failed with error code ");
        Serial.println(ota_download);
        return;
    }
    Serial.print(ota_download);
    Serial.println(" bytes stored.");

    Serial.println("Verify update integrity and apply ...");
    if ((ota_err = ota.update()) != Arduino_ESP32_OTA::Error::None)
    {
        Serial.print("ota.update() failed with error code ");
        Serial.println((int)ota_err);
        return;
    }

    Serial.println("Performing a reset after which the bootloader will start the new firmware.");

    delay(1000);

    ota.reset();
}