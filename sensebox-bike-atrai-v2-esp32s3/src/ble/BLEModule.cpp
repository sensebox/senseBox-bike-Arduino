#include "BLEModule.h"

bool isConnectedVar = false;
BLEServer *pServer;
BLEService *pService;

char macString[32]; // Enough space for the MAC string

class CustomBLECallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("Received data:");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }
      Serial.println();
    }
  }
};

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

// for receiving data
int BLEModule::createCharacteristicWithCallback(const char *uuid) {
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        uuid,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_INDICATE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Attach the callback to handle incoming data
    pCharacteristic->setCallbacks(new CustomBLECallbacks());
    return 1;
}

bool BLEModule::writeBLE(const char * characteristicId, float value)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    pCharacteristic->setValue(value);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, float value, float value2)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[8];
    memcpy(&buf[0], &value, sizeof(float));
    memcpy(&buf[4], &value2, sizeof(float));
    pCharacteristic->setValue(buf,2);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, float value, float value2, float value3)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[12];
    memcpy(&buf[0], &value, sizeof(float));
    memcpy(&buf[4], &value2, sizeof(float));
    memcpy(&buf[8], &value3, sizeof(float));
    pCharacteristic->setValue(buf,3);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, float value, float value2, float value3, float value4)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[16];
    memcpy(&buf[0], &value, sizeof(float));
    memcpy(&buf[4], &value2, sizeof(float));
    memcpy(&buf[8], &value3, sizeof(float));
    memcpy(&buf[12], &value4, sizeof(float));
    pCharacteristic->setValue(buf,4);
    pCharacteristic->notify();
    return true;
}

bool BLEModule::writeBLE(const char * characteristicId, float value, float value2, float value3, float value4, float value5)
{
    BLECharacteristic *pCharacteristic = pService->getCharacteristic(characteristicId);
    uint8_t buf[20];
    memcpy(&buf[0], &value, sizeof(float));
    memcpy(&buf[4], &value2, sizeof(float));
    memcpy(&buf[8], &value3, sizeof(float));
    memcpy(&buf[12], &value4, sizeof(float));
    memcpy(&buf[16], &value5, sizeof(float));
    pCharacteristic->setValue(buf,5);
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
