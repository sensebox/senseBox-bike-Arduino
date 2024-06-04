#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "../Sensor.h"

class DustSensor : public Sensor
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

  static const String pmUUID;

  static void sensorTask(void *pvParameters);
  void notifyBLE(float pm1, float pm2_5, float pm4, float pm10);
};

#endif // DUST_SENSOR_H
