#include "AccelerationSensor.h"

AccelerationSensor::AccelerationSensor() : BaseSensor("accelerationSensorTask", 
4096, // taskStackSize,
1, // taskDelay,
17, // taskPriority,
1 // core
) {}

String surfaceClassificationUUID = "b944af10-f495-4560-968f-2f0d18cab521";
// String accUUID = "B944AF10F4954560968F2F0D18CAB522";
String anomalyUUID = "b944af10-f495-4560-968f-2f0d18cab523";
int surfaceClassificationCharacteristic = 0;
int anomalyCharacteristic = 0;

Adafruit_MPU6050 mpu;

void AccelerationSensor::initSensor()
{
    Serial.println("setting up MPU6050...");

  if (!mpu.begin(0x68, &Wire))
  {
    Serial.println("MPU6050 Chip wurde nicht gefunden");
    delay(500);
    return;
  }
  else
  {
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    delay(100);
  };

  surfaceClassificationCharacteristic = BLEModule::createCharacteristic(surfaceClassificationUUID.c_str());
  anomalyCharacteristic = BLEModule::createCharacteristic(anomalyUUID.c_str());
}

int acc_input_frame_size = impulse_440960_0.dsp_input_frame_size;
float* buffer = new float[acc_input_frame_size]();
size_t ix = 0;
float probAsphalt = 0.0;
float probCompact = 0.0;
float probPaving = 0.0;
float probSett = 0.0;
float probStanding = 0.0;
float anomaly = 0.0;

static int get_signal_data_440960(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (buffer + offset)[i];
    }
    return EIDSP_OK;
}

unsigned long startAccTime = millis();
bool AccelerationSensor::readSensorData()
{
  bool classified = false;
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  buffer[ix++] = 1.0;
  buffer[ix++] = 1.0;
  buffer[ix++] = 1.0;
  buffer[ix++] = 1.0;
  buffer[ix++] = 1.0;
  buffer[ix++] = 1.0;

  unsigned long endAccTime = millis();
  if ((endAccTime - startAccTime) < 30)
  {
    vTaskDelay(pdMS_TO_TICKS(30 - (endAccTime - startAccTime)));
  }
  Serial.printf("acceleration: %lu ms\n", endAccTime - startAccTime);
  startAccTime = millis();

  // one second inverval
  if (acc_input_frame_size <= ix)
  {
    classified = true;
    // Turn the raw buffer in a signal which we can the classify
    signal_t signal;
    signal.total_length = acc_input_frame_size;
    signal.get_data = &get_signal_data_440960;
    // int err = numpy::signal_from_buffer(buffer, acc_input_frame_size, &signal);
    // if (err != 0)
    // {
    //   ei_printf("Failed to create signal from buffer (%d)\n", err);
    //   buffer[acc_input_frame_size] = {};
    //   return classified;
    // }

    // Run the classifier
    ei_impulse_result_t result = {};

    int err = process_impulse(&impulse_handle_440960_0, &signal, &result, false);
    if (err != EI_IMPULSE_OK)
    {
      ei_printf("ERR: Failed to run classifier (%d)\n", err);
      buffer[acc_input_frame_size] = {};
      return classified;
    }

    probAsphalt = result.classification[0].value;
    probCompact = result.classification[1].value;
    probPaving = result.classification[2].value;
    probSett = result.classification[3].value;
    probStanding = result.classification[4].value;

    anomaly = result.anomaly;
    Serial.printf("Asphalt: %f, Compact: %f, Paving: %f, Sett: %f, Standing: %f, Anomaly: %f\n", probAsphalt, probCompact, probPaving, probSett, probStanding, anomaly);

    if (sendBLE)
    {
      notifyBLE(probAsphalt, probCompact, probPaving, probSett, probStanding, anomaly);
    }

    ix = 0;

    buffer[acc_input_frame_size] = {};

    startAccTime = millis();
  }

  if (measurementCallback)
  {
    measurementCallback({probAsphalt, probCompact, probPaving, probSett, probStanding});
  }

  return classified;
}

void AccelerationSensor::notifyBLE(float probAsphalt, float probCompact, float probPaving, float probSett, float probStanding, float anomaly)
{
  BLEModule::writeBLE(surfaceClassificationUUID.c_str(), probAsphalt*100, probCompact*100, probPaving*100, probSett*100, probStanding*100);
  BLEModule::writeBLE(anomalyUUID.c_str(), anomaly);
}