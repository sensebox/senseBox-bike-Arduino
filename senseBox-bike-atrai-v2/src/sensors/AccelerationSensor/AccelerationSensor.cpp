#include "AccelerationSensor.h"

AccelerationSensor::AccelerationSensor() : BaseSensor("accelerationSensorTask", 2048, 100) {}

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

void AccelerationSensor::readSensorData()
{
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  float x = a.acceleration.x;
  float y = a.acceleration.y;
  float z = a.acceleration.z;

  if (measurementCallback)
  {
    measurementCallback({x, y, z});
  }

  if (sendBLE)
  {
    notifyBLE(x, y, z);
  }
}

void AccelerationSensor::notifyBLE(float x, float y, float z)
{
  BLEModule::writeBLE(accCharacteristic, x, y, z);
}