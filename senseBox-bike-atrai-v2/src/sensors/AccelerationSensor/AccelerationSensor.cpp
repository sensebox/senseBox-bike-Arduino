#include "AccelerationSensor.h"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

AccelerationSensor::AccelerationSensor() : BaseSensor("accelerationSensorTask", 2048, 0) {}

String accUUID = "B944AF10F4954560968F2F0D18CAB522";
int accCharacteristic = 0;

Adafruit_MPU6050 mpu;

void AccelerationSensor::initSensor()
{

  if (!mpu.begin(0x68, &Wire1))
  {
    Serial.println("MPU6050 Chip wurde nicht gefunden");
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

  accCharacteristic = BLEModule::createCharacteristic(accUUID.c_str());
}

float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {};
size_t ix = 0;
float probAsphalt = 0.0;
float probCompact = 0.0;
float probPaving = 0.0;
float probSett = 0.0;
float probStanding = 0.0;

float prevAccTime = millis();

void AccelerationSensor::readSensorData()
{
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  buffer[ix++] = a.acceleration.x;
  buffer[ix++] = a.acceleration.y;
  buffer[ix++] = a.acceleration.z;
  buffer[ix++] = g.gyro.x;
  buffer[ix++] = g.gyro.y;
  buffer[ix++] = g.gyro.z;

  Serial.println(millis() - prevAccTime);
  prevAccTime = millis();

  // one second inverval
  if (EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE <= ix)
  {
    // Turn the raw buffer in a signal which we can the classify
    signal_t signal;
    int err = numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0)
    {
      ei_printf("Failed to create signal from buffer (%d)\n", err);
      return;
    }

    // Run the classifier
    ei_impulse_result_t result = {};

    err = run_classifier(&signal, &result, false);
    if (err != EI_IMPULSE_OK)
    {
      ei_printf("ERR: Failed to run classifier (%d)\n", err);
      return;
    }

    probAsphalt = result.classification[0].value;
    probCompact = result.classification[1].value;
    probPaving = result.classification[2].value;
    probSett = result.classification[3].value;
    probStanding = result.classification[4].value;

    if (sendBLE)
    {

      notifyBLE(probAsphalt, probCompact, probPaving, probSett, probStanding);
    }

    ix = 0;

    buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {};
  }

  if (measurementCallback)
  {
    measurementCallback({probAsphalt, probCompact, probPaving, probSett, probStanding});
  }
}

void AccelerationSensor::notifyBLE(float probAsphalt, float probCompact, float probPaving, float probSett, float probStanding)
{
  BLEModule::writeBLE(accCharacteristic, probAsphalt, probCompact, probPaving, probSett, probStanding);
}