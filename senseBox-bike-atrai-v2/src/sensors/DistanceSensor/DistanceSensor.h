#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include "../Sensor.h"

class DistanceSensor : public Sensor
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

  static const String distanceUUID;

  static void sensorTask(void *pvParameters);
  void notifyBLE(float distance);
};

#endif // DISTANCE_SENSOR_H
