#include "LuxSensor.h"

LuxSensor::LuxSensor() : BaseSensor("LuxSensorTask", 2048, 1000) {}

String luxUUID = "7E14E07084EA489FB45AE1317364B979";
int luxCharacteristic = 0;

Adafruit_LTR329 ltr = Adafruit_LTR329();

void LuxSensor::initSensor()
{
  if ( ! ltr.begin() ) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setGain(LTR3XX_GAIN_2);
  ltr.setIntegrationTime(LTR3XX_INTEGTIME_100);
  ltr.setMeasurementRate(LTR3XX_MEASRATE_500);

  luxCharacteristic = BLEModule::createCharacteristic(luxUUID.c_str());
}

bool LuxSensor::readSensorData()
{
  Wire.setClock(100000); // Sensor has max I2C freq of 1MHz
  bool valid;
  uint16_t visible_plus_ir, infrared;
  if (ltr.newDataAvailable()) {
    valid = ltr.readBothChannels(visible_plus_ir, infrared);
    if (valid) {
      float lux = (float)visible_plus_ir - (float)infrared;
      if (lux < 0.0) {
        lux = 0.0;
      }
      if (measurementCallback)
      {
        measurementCallback({lux});
      }
      if (sendBLE)
      {
        notifyBLE(lux);
      }
    }
  }
  return false;
}

void LuxSensor::notifyBLE(float lux)
{
  BLEModule::writeBLE(luxCharacteristic, lux, lux, lux, lux);
}