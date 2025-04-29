#ifndef ACCELERATION_SENSOR_H
#define ACCELERATION_SENSOR_H

#include <functional>
#include <vector>
#include "../ble/BLEModule.h"

#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <vl53l8cx_class.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class AccelerationDistanceSensor
{
public:
  AccelerationDistanceSensor();
  bool accReadSensorData();
  bool distReadSensorData();

  void begin();
  void subscribe(std::function<void(std::vector<float>)> callback);
  void startSubscription();
  void stopSubscription();
  void startBLE();
  void stopBLE();

protected:
  void accInitSensor();
  void distInitSensor();
  void accNotifyBLE(float probAsphalt, float probCompact, float probPaving, float probSett, float probStanding, float anomaly);
  void distNotifyBLE(float distance, float overtakingPredictionPercentage);
  void distNotifyBLERight(float distance, float overtakingPredictionPercentage);
  static void accTask(void* pvParameters);
  static void distTask(void* pvParameters);
  bool activeSubscription;
  bool sendBLE;
  std::function<void(std::vector<float>)> measurementCallback;

private:
  TaskHandle_t accHandle;
  TaskHandle_t distHandle;
  void tcaselect(uint8_t i);
};

#endif // ACCELERATION_SENSOR_H
