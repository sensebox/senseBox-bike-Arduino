#include "DeviceInfo.h"

String serviceUUID = "CF06A218F68EE0BEAD048EBC1EB0BC85";
String modelCharacteristicUUID = "2CDF217435BEFDC44CA26FD173F8B3A9";
String versionCharacteristicUUID = "2CDF217435BEFDC44CA26FD173F8B3B0";

int modelCharacteristic = 0;
int versionCharacteristic = 0;

DeviceInfo::DeviceInfo(const String model, const String version, uint32_t taskDelay)
    : taskStackSize(taskStackSize), taskDelay(taskDelay)
{
    this->model = model;
    this->version = version;
}

void DeviceInfo::begin()
{
    BLEModule::createService(serviceUUID.c_str());
    modelCharacteristic = BLEModule::createCharacteristic(modelCharacteristicUUID.c_str());
    versionCharacteristic = BLEModule::createCharacteristic(versionCharacteristicUUID.c_str());
}

void DeviceInfo::notifyBLE()
{

    BLEModule::writeBLE(modelCharacteristic, this->model);
    BLEModule::writeBLE(versionCharacteristic, this->version);
}
