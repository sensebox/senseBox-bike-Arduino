#ifndef BASESENSOR_H
#define BASESENSOR_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include "../ble/BLEModule.h"

class BaseSensor
{
public:
    BaseSensor(const char *taskName, uint32_t taskStackSize = 8192, uint32_t taskDelay = 1000);

    String name;

    void begin();
    void subscribe(std::function<void(std::vector<float>)> callback);
    void startSubscription();
    void stopSubscription();
    void startBLE();
    void stopBLE();
    bool isInitialized();

protected:
    bool initialized = false;
    const int maxInitAttempts = 5;
    virtual void initSensor() = 0;
    virtual bool readSensorData() = 0;
    static void sensorTask(void *pvParameters);
    bool activeSubscription;
    bool sendBLE;
    std::function<void(std::vector<float>)> measurementCallback;

private:
    const char *taskName;
    uint32_t taskStackSize;
    uint32_t taskDelay;
};

#endif // BASESENSOR_H
