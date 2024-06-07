#include "BLEModule.h"
#include "Utf16Converter/Utf16Converter.h"

void (*BLEModule::receiveCallback)(String) = nullptr;

void BLEModule::receivedData()
{
    int size = 32;
    uint8_t receivedData[size];
    SenseBoxBLE::read(receivedData, size);

    String data = utf16leToString(receivedData, size);

    BLEModule::receiveCallback(data);
}

BLEModule::BLEModule()
{
    bleName = "";
}

bool BLEModule::begin()
{
    SenseBoxBLE::start("senseBox-BLE");
    delay(500);
    bleName = "senseBox:bike [" + SenseBoxBLE::getMCUId() + "]";
    SenseBoxBLE::setName(bleName);

    delay(200);
    SenseBoxBLE::addService("CF06A218F68EE0BEAD048EBC1EB0BC84");

    delay(200);
    SenseBoxBLE::configHandler = &receivedData;

    int configCharacteristic = SenseBoxBLE::setConfigCharacteristic("29BD0A8551E44D3C914E126541EB2A5E", "60B1D5CE353944D2BB35FF2DAABE17FF");

    xTaskCreate(bleTask, "bleTask", 2048, NULL, 1, NULL);

    return true;
}

String BLEModule::getBLEName()
{
    return bleName;
}

const char **BLEModule::getBLEConnectionString()
{
    String bleId = "[" + SenseBoxBLE::getMCUId() + "]";
    String bleIdBegin = bleId.substring(0, bleId.length() / 2);
    String bleIdEnd = bleId.substring(bleId.length() / 2);
    const char *MESSAGE_CONFIGURE_WIFI[] = {
        "senseBox",
        "bike",
        bleIdBegin.c_str(),
        bleIdEnd.c_str()};
    return MESSAGE_CONFIGURE_WIFI;
}

int BLEModule::createCharacteristic(const char *uuid)
{
    return SenseBoxBLE::addCharacteristic(uuid);
}

bool BLEModule::writeBLE(int characteristicId, float value)
{
    return SenseBoxBLE::write(characteristicId, value);
}

bool BLEModule::writeBLE(int characteristicId, float value, float value2)
{
    return SenseBoxBLE::write(characteristicId, value, value2);
}

bool BLEModule::writeBLE(int characteristicId, float value, float value2, float value3)
{
    return SenseBoxBLE::write(characteristicId, value, value2, value3);
}

bool BLEModule::writeBLE(int characteristicId, float value, float value2, float value3, float value4)
{
    return SenseBoxBLE::write(characteristicId, value, value2, value3, value4);
}

bool BLEModule::writeBLE(int characteristicId, float value, float value2, float value3, float value4, float value5)
{
    return SenseBoxBLE::write(characteristicId, value, value2, value3, value4, value5);
}

void BLEModule::setReceiveCallback(void (*callback)(String data))
{
    BLEModule::receiveCallback = callback;
}

void BLEModule::bleTask(void *pvParameters)
{
    while (true)
    {
        SenseBoxBLE::poll();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
