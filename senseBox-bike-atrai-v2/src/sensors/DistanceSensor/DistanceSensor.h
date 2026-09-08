#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include "../BaseSensor.h"
#include <Wire.h>
#include <vl53l8cx.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// #include <TensorFlowLite_ESP32.h>
#include "edge-impulse-sdk/tensorflow/lite/micro/kernels/micro_ops.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/micro_interpreter.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/all_ops_resolver.h"
#include "edge-impulse-sdk/tensorflow/lite/schema/schema_generated.h"
#include <edge-impulse-sdk/tensorflow/lite/micro/micro_error_reporter.h>

class DistanceSensor : public BaseSensor
{
public:
  DistanceSensor();
  bool readSensorData() override;
  static bool isPresent();

protected:
  void initSensor() override;
  void notifyBLE(float distance, float overtakingPredictionPercentage, float bikeOvertakingPredictionPercentage);
};

#endif // DISTANCESENSOR_H