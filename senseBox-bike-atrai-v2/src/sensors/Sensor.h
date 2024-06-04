#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include "../ble/BLEModule.h"

class Sensor
{
public:
  virtual void begin() = 0;
  virtual void subscribe(std::function<void(String, std::vector<float>)> callback) = 0;
  virtual void startSubscription() = 0;
  virtual void stopSubscription() = 0;
  virtual void startBLE() = 0;
  virtual void stopBLE() = 0;

protected:
  bool sendBLE;
  bool activeSubscription;

  int characteristicId;
  void setBLEStatus(bool status);
};

#endif // SENSOR_H
