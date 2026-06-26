#include "LuxSensor.h"

LuxSensor::LuxSensor() : BaseSensor("LuxSensorTask", 2048, 1000) {}

String luxUUID = "7E14E07084EA489FB45AE1317364B979";
int luxCharacteristic = 0;

Adafruit_LTR329 ltr = Adafruit_LTR329();

float LuxSensor::calculateLux(uint16_t ch0, uint16_t ch1)
{
    // Avoid division by zero
    if (ch0 == 0) return 0.0f;

    // Gain divisor — must match ltr.setGain()
    // LTR3XX_GAIN_1=1, GAIN_2=2, GAIN_4=4, GAIN_8=8, GAIN_48=48, GAIN_96=96
    float gainFactor = 1.0f;  // matches LTR3XX_GAIN_1

    // Integration time divisor — must match ltr.setIntegrationTime()
    // 50ms→0.5, 100ms→1.0, 150ms→1.5, 200ms→2.0, 250ms→2.5, 300ms→3.0, 350ms→3.5, 400ms→4.0
    float integFactor = 0.5f;  // matches LTR3XX_INTEGTIME_50

    float ratio = (float)ch1 / ((float)ch0 + (float)ch1);

    float lux = 0.0f;
    if (ratio < 0.45f) {
        lux = (1.7743f * ch0 + 1.1059f * ch1) / gainFactor / integFactor;
    } else if (ratio < 0.64f) {
        lux = (4.2785f * ch0 - 1.9548f * ch1) / gainFactor / integFactor;
    } else if (ratio < 0.85f) {
        lux = (0.5926f * ch0 + 0.1185f * ch1) / gainFactor / integFactor;
    } else {
        lux = 0.0f;  // Unreliable — IR overwhelms visible
    }

    return max(lux, 0.0f);
}

void LuxSensor::initSensor()
{
  if ( ! ltr.begin() ) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setGain(LTR3XX_GAIN_1);
  ltr.setIntegrationTime(LTR3XX_INTEGTIME_50);
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
    if (visible_plus_ir >= 64000 || infrared >= 64000) {
      Serial.println("Sensor saturated (direct sunlight)");
      float lux = 64000.0f;
      if (measurementCallback) measurementCallback({lux});
      if (sendBLE) notifyBLE(lux);
      return false;
    }
    float lux = calculateLux(visible_plus_ir, infrared);
    if (lux < 0.0) lux = 0.0;
    if (measurementCallback) measurementCallback({lux});
    if (sendBLE) notifyBLE(lux);
  }
  return false;
}

void LuxSensor::notifyBLE(float lux)
{
  Serial.print("Notifying BLE with Lux: "); Serial.println(lux);
  BLEModule::writeBLE(luxCharacteristic, lux, lux, lux, lux);
}