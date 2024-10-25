#include "BLEModule.h"

bool isConnectedVar = false;
BLEServer *pServer;
BLEService *pService;


BLEModule::BLEModule()
{
    bleName = "";
}

bool BLEModule::begin()
{
    BLEDevice::init("senseBox-BLE[xxxyyy]"); // not sure how to adjust the id
    pServer = BLEDevice::createServer();

    return true;
}

String BLEModule::getBLEName()
{
    return bleName;
}

bool BLEModule::isConnected()
{
    return isConnectedVar;
}

const char **BLEModule::getBLEConnectionString()
{
    String bleId = "[xxxyyy]";
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
    pService = pServer->createService(uuid);
    pService->start();
    return 1;
}

int BLEModule::createCharacteristic(const char *uuid)
{
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(uuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    return 1;
}

bool BLEModule::writeBLE(const char * characteristicId, float value)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    pCharacteristic->setValue(value);
    // pCharacteristic->notify(); ??
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[2] = {value, value2};
    pCharacteristic->setValue(buf,1);
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2, uint8_t value3)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[3] = {value, value2, value3};
    pCharacteristic->setValue(buf,1);
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2, uint8_t value3, uint8_t value4)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[4] = {value, value2, value3, value4};
    pCharacteristic->setValue(buf,1);
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2, uint8_t value3, uint8_t value4, uint8_t value5)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[5] = {value, value2, value3, value4};
    pCharacteristic->setValue(buf,1);
    return true;
}

void BLEModule::bleTask()
{
    while (true)
    {  
        BLEDevice::startAdvertising();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void BLEModule::bleStartPoll(const char *uuid)
{
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(uuid);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}
