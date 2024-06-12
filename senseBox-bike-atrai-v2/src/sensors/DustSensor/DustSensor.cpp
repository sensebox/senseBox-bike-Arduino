#include "DustSensor.h"

DustSensor::DustSensor() : BaseSensor("DustSensorTask", 2048, 1000) {}

String dustUUID = "7E14E07084EA489FB45AE1317364B979";
int dustCharacteristic = 0;

// add more if needed
long prevDustTime = millis();

void DustSensor::initSensor()
{
  int16_t ret;
  uint8_t auto_clean_days = 4;
  uint32_t auto_clean;

  sensirion_i2c_init();

  while (sps30_probe() != 0)
  {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
  }

  Serial.print("SPS sensor probing successful\n");

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

  Serial.print("measurements started\n");

  dustCharacteristic = BLEModule::createCharacteristic(dustUUID.c_str());
}

void DustSensor::readSensorData()
{
  struct sps30_measurement m;
  char serial[SPS30_MAX_SERIAL_LEN];
  uint16_t data_ready;
  int16_t ret;

  // retry 5 times until the sensor has data ready
  int retries = 5;
  int retryCount = 0;
    ret = sps30_read_data_ready(&data_ready);
    if (ret < 0)
    {
      Serial.print("error reading data-ready flag: ");
      Serial.println(ret);
    }
    else if (!data_ready)
      Serial.print("data not ready, no new measurement available\n");
    else

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

        if (measurementCallback)
        {
          measurementCallback({pm1, pm2_5, pm4, pm10});
        }

        if (sendBLE)
        {
          notifyBLE(pm1, pm2_5, pm4, pm10);
        }
      }
  
  Serial.println('dust:' + String(millis() - prevDustTime));
  prevDustTime = millis();
}

void DustSensor::notifyBLE(float pm1, float pm2_5, float pm4, float pm10)
{
  BLEModule::writeBLE(dustCharacteristic, pm1, pm2_5, pm4, pm10);
}