#include "BLEModule.h"

bool isConnectedVar = false;
BLEServer *pServer;
BLEService *pService;

char macString[32]; // Enough space for the MAC string

BLEModule::BLEModule()
{
    bleName = "";
}

bool BLEModule::begin()
{
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_BT);
    snprintf(macString, sizeof(macString), "%02X:%02X:%02X:%02X:%02X:%02X",
             baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    char bleName[64];
    snprintf(bleName, sizeof(bleName), "senseBox-BLE[%s]", macString);

    BLEDevice::init(bleName);
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
    char bleId[34];
    snprintf(bleId, sizeof(bleId), "[%s]", macString);
    std::string bleIdStr = bleId;
    std::string bleIdBegin = bleIdStr.substr(0, bleIdStr.length() / 2);
    std::string bleIdEnd = bleIdStr.substr(bleIdStr.length() / 2);
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
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(uuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_INDICATE| BLECharacteristic::PROPERTY_NOTIFY);
    return 1;
}

bool BLEModule::writeBLE(const char * characteristicId, float value)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    pCharacteristic->setValue(value);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[2] = {value, value2};
    pCharacteristic->setValue(buf,1);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2, uint8_t value3)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[3] = {value, value2, value3};
    pCharacteristic->setValue(buf,1);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2, uint8_t value3, uint8_t value4)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[4] = {value, value2, value3, value4};
    pCharacteristic->setValue(buf,1);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, uint8_t value, uint8_t value2, uint8_t value3, uint8_t value4, uint8_t value5)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[5] = {value, value2, value3, value4};
    pCharacteristic->setValue(buf,1);
    pCharacteristic->notify();
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

const char *BLEModule::getMacAddress()
{
    return macString; // Enough space for the MAC string;
}
