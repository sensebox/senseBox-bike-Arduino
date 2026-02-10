#ifndef BLE_MODULE_H
#define BLE_MODULE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

class BLEModule
{
public:
    BLEModule();

    // Initialize the BLE module
    bool begin();

    // Get the BLE module ID
    String getBLEName();

    void bleStartPoll(const char *uuid);

    const char **getBLEConnectionString();

    static int createService(const char *uuid);

    // Create a BLE characteristic
    static int createCharacteristic(const char *uuid);
    static int createCharacteristicWithCallback(const char *uuid);

    static bool writeBLE(const char * characteristicId, float value);
    static bool writeBLE(const char * characteristicId, float value, float value2);
    static bool writeBLE(const char * characteristicId, float value, float value2, float value3);
    static bool writeBLE(const char * characteristicId, float value, float value2, float value3, float value4);
    static bool writeBLE(const char * characteristicId, float value, float value2, float value3, float value4, float value5);

    // Set callback for receiving data
    // void setReceiveCallback(void (*callback)(BLEDevice, BLECharacteristic));

    // Task function for polling BLE
    static void bleTask();

    static bool isConnected();

    const char *getMacAddress();

private:
    // BLEService* service;
    String bleName;

    // static void onReceive(BLEDevice central, BLECharacteristic characteristic);
    // static void (*receiveCallback)(BLEDevice, BLECharacteristic);
};

#endif // BLE_MODULE_H
