#include "BLEModule.h"

// void (*BLEModule::receiveCallback)(BLEDevice, BLECharacteristic) = nullptr;

BLEModule::BLEModule()
{
    // service = nullptr;
    bleName = "";
}

bool BLEModule::begin()
{
    SenseBoxBLE::start("senseBox-BLE");
    delay(500);
    bleName = "senseBox:bike [" + SenseBoxBLE::getMCUId() + "]";
    SenseBoxBLE::setName(bleName);

    delay(200);
    // SenseBoxBLE::addService("CF06A218F68EE0BEAD048EBC1EB0BC84");

    xTaskCreate(bleTask, "bleTask", 1024, NULL, 1, NULL);

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

int BLEModule::createService(const char *uuid)
{
    return SenseBoxBLE::addService(uuid);
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

// void BLEModule::setReceiveCallback(void (*callback)(BLEDevice, BLECharacteristic)) {
//     receiveCallback = callback;
// }

// void BLEModule::onReceive(BLEDevice central, BLECharacteristic characteristic) {
//     if (receiveCallback) {
//         receiveCallback(central, characteristic);
//     }
// }

void BLEModule::bleTask(void *pvParameters)
{
    while (true)
    {
        SenseBoxBLE::poll();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
