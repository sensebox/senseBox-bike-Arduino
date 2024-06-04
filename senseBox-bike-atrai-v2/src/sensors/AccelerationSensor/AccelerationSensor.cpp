#include "AccelerationSensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Adafruit_MPU6050.h>

const String AccelerationSensor::accUUID = "B944AF10F4954560968F2F0D18CAB522";

Adafruit_MPU6050 mpu;

int accCharacteristic = 0;

void AccelerationSensor::begin()
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

  sendBLE = false;
  activeSubscription = true;
  xTaskCreate(sensorTask, "AccelerationSensorTask", 2048, this, 1, NULL);
}

void AccelerationSensor::subscribe(std::function<void(String, std::vector<float>)> callback)
{
  this->measurementCallback = callback;
}

void AccelerationSensor::startSubscription()
{
  activeSubscription = true;
}

void AccelerationSensor::stopSubscription()
{
  activeSubscription = false;
}

void AccelerationSensor::startBLE()
{
  setBLEStatus(true);
}

void AccelerationSensor::stopBLE()
{
  setBLEStatus(false);
}

void AccelerationSensor::sensorTask(void *pvParameters)
{
  AccelerationSensor *sensor = static_cast<AccelerationSensor *>(pvParameters);

  while (true)
  {
    if (sensor->activeSubscription)
    {

      sensors_event_t a, g, temp;

      mpu.getEvent(&a, &g, &temp);

      float x = a.acceleration.x;
      float y = a.acceleration.y;
      float z = a.acceleration.z;

      if (sensor->measurementCallback)
      {
        sensor->measurementCallback(sensor->uuid, {x, y, z});
      }

      if (sensor->sendBLE)
      {
        sensor->notifyBLE(x, y, z);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void AccelerationSensor::notifyBLE(float x, float y, float z)
{
  // print characteristic
  Serial.print("Acceleration Characteristic: ");
  Serial.println(accCharacteristic);
  Serial.printf("Acceleration: x: %f, y: %f, z: %f\n", x, y, z);

  BLEModule::writeBLE(accCharacteristic, x, y, z);
}
