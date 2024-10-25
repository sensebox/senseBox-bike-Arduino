#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include "../ble/BLEModule.h"

class DeviceInfo
{
public:
    DeviceInfo(const String model, const String version, uint32_t taskDelay = 1000);
    void begin();
    void notifyBLE();

protected:
private:
    String model;
    String version;
    const char *taskName;
    uint32_t taskStackSize;
    uint32_t taskDelay;
};

#endif // DEVICEINFO_H
