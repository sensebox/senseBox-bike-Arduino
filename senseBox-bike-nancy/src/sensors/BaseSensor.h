#ifndef BASESENSOR_H
#define BASESENSOR_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include "../ble/BLEModule.h"
#include <Wire.h>

class BaseSensor
{
public:
    BaseSensor(const char *taskName, uint32_t taskStackSize = 8192, uint32_t taskDelay = 1000);

    void begin();
    void subscribe(std::function<void(std::vector<float>)> callback);
    void startSubscription();
    void stopSubscription();
    void startBLE();
    void stopBLE();

protected:
    virtual void initSensor() = 0;
    virtual bool readSensorData() = 0;
    static void sensorTask(void *pvParameters);
    bool activeSubscription;
    bool sendBLE;
    std::function<void(std::vector<float>)> measurementCallback;
    void tcaselect(uint8_t i);

private:
    const char *taskName;
    uint32_t taskStackSize;
    uint32_t taskDelay;
};

#endif // BASESENSOR_H
