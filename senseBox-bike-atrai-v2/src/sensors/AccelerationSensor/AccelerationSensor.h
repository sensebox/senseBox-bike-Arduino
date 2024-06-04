#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include "../Sensor.h"

class AccelerationSensor : public Sensor
{
public:
  void begin() override;
  void subscribe(std::function<void(String, std::vector<float>)> callback) override;
  void startSubscription() override;
  void stopSubscription() override;
  void startBLE() override;
  void stopBLE() override;

private:
  std::function<void(String, std::vector<float>)> measurementCallback;
  String uuid;

  static const String accUUID;

  static void sensorTask(void *pvParameters);
  void notifyBLE(float x, float y, float z);
};

#endif // ACCELERATION_SENSOR_H
