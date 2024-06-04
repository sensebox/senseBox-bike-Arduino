#ifndef TEMP_HUMIDITY_SENSOR_H
#define TEMP_HUMIDITY_SENSOR_H

#include "../Sensor.h"

class TempHumiditySensor : public Sensor
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

  static const String tempUUID;
  static const String humUUID;

  static void sensorTask(void *pvParameters);
  void notifyBLE(float temperature, float humidity);
};

#endif // TEMP_HUMIDITY_SENSOR_H
