#include "TempHumiditySensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "Adafruit_HDC1000.h"

const String TempHumiditySensor::tempUUID = "2CDF217435BEFDC44CA26FD173F8B3A8";
const String TempHumiditySensor::humUUID = "772DF7EC8CDC4EA986AF410ABE0BA257";

int temperatureCharacteristic = 0;
int humidityCharacteristic = 0;

Adafruit_HDC1000 hdc = Adafruit_HDC1000();

void TempHumiditySensor::begin()
{
  if (!hdc.begin())
  {
    Serial.println("Couldn't find HDC1080 sensor!");
    while (1)
      ;
  }

  temperatureCharacteristic = BLEModule::createCharacteristic(tempUUID.c_str());
  humidityCharacteristic = BLEModule::createCharacteristic(humUUID.c_str());

  sendBLE = false;
  activeSubscription = true;
  xTaskCreate(sensorTask, "TempHumiditySensorTask", 2048, this, 1, NULL);
}

void TempHumiditySensor::subscribe(std::function<void(String, std::vector<float>)> callback)
{
  this->measurementCallback = callback;
}

void TempHumiditySensor::startSubscription()
{
  activeSubscription = true;
}

void TempHumiditySensor::stopSubscription()
{
  activeSubscription = false;
}

void TempHumiditySensor::startBLE()
{
  setBLEStatus(true);
}

void TempHumiditySensor::stopBLE()
{
  setBLEStatus(false);
}

void TempHumiditySensor::sensorTask(void *pvParameters)
{
  TempHumiditySensor *sensor = static_cast<TempHumiditySensor *>(pvParameters);

  while (true)
  {
    if (sensor->activeSubscription)
    {
      float temperature = hdc.readTemperature();
      float humidity = hdc.readHumidity();

      if (sensor->measurementCallback)
      {
        sensor->measurementCallback(sensor->uuid, {temperature, humidity});
      }

      if (sensor->sendBLE)
      {
        sensor->notifyBLE(temperature, humidity);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void TempHumiditySensor::notifyBLE(float temperature, float humidity)
{
  Serial.println("Notifying BLE");
  Serial.println(temperature);
  Serial.println(temperatureCharacteristic);

  BLEModule::writeBLE(temperatureCharacteristic, temperature);
  BLEModule::writeBLE(humidityCharacteristic, humidity);
}
