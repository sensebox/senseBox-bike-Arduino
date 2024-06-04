#include "DustSensor.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sps30.h>

const String DustSensor::pmUUID = "7E14E07084EA489FB45AE1317364B979";

int dustCharacteristic = 0;

void DustSensor::begin()
{
  int16_t ret;
  uint8_t auto_clean_days = 4;
  uint32_t auto_clean;

  Serial.println("Dust Setup");

  sensirion_i2c_init();

  while (sps30_probe() != 0)
  {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
  }

  ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
  if (ret)
  {
    Serial.print("error setting the auto-clean interval: ");
    Serial.println(ret);
  }

  ret = sps30_start_measurement();
  if (ret < 0)
  {
    Serial.print("error starting measurement\n");
  }

  dustCharacteristic = BLEModule::createCharacteristic(pmUUID.c_str());

  sendBLE = false;
  activeSubscription = true;

  xTaskCreate(sensorTask, "DustSensorTask", 2048, this, 1, NULL);
}

void DustSensor::subscribe(std::function<void(String, std::vector<float>)> callback)
{
  this->measurementCallback = callback;
}

void DustSensor::startSubscription()
{
  activeSubscription = true;
}

void DustSensor::stopSubscription()
{
  activeSubscription = false;
}

void DustSensor::startBLE()
{
  setBLEStatus(true);
}

void DustSensor::stopBLE()
{
  setBLEStatus(false);
}

void DustSensor::sensorTask(void *pvParameters)
{
  DustSensor *sensor = static_cast<DustSensor *>(pvParameters);

  while (true)
  {
    if (sensor->activeSubscription)
    {

      struct sps30_measurement m;
      char serial[SPS30_MAX_SERIAL_LEN];
      uint16_t data_ready;
      int16_t ret;

      do
      {
        ret = sps30_read_data_ready(&data_ready);
        if (ret < 0)
        {
          Serial.print("error reading data-ready flag: ");
          Serial.println(ret);
        }
        else if (!data_ready)
          Serial.print("data not ready, no new measurement available\n");
        else
          break;
        delay(100); /* retry in 100ms */
      } while (1);

      ret = sps30_read_measurement(&m);
      if (ret < 0)
      {
        Serial.print("error reading measurement\n");
      }
      else
      {
        float pm1 = m.mc_1p0;
        float pm2_5 = m.mc_2p5;
        float pm4 = m.mc_4p0;
        float pm10 = m.mc_10p0;

        if (sensor->measurementCallback)
        {
          sensor->measurementCallback(sensor->uuid, {pm1, pm2_5, pm4, pm10});
        }

        if (sensor->sendBLE)
        {
          sensor->notifyBLE(pm1, pm2_5, pm4, pm10);
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void DustSensor::notifyBLE(float pm1, float pm2_5, float pm4, float pm10)
{
  Serial.print("Dust Characteristic: ");
  Serial.println(dustCharacteristic);
  Serial.printf("Dust: PM1: %f, PM2.5: %f, PM4: %f, PM10: %f\n", pm1, pm2_5, pm4, pm10);
  BLEModule::writeBLE(characteristicId, pm1, pm2_5, pm4, pm10);
}
