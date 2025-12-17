#include "DustSensor.h"

DustSensor::DustSensor() : BaseSensor("DustSensorTask", 2048, 1000) {}

String dustUUID = "7E14E07084EA489FB45AE1317364B979";
int dustCharacteristic = 0;

SensirionI2cSps30 sensor;
static char errorMessage[64];
static int16_t error;

#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

void DustSensor::initSensor()
{
  sensor.begin(Wire, SPS30_I2C_ADDR_69);
  sensor.stopMeasurement();
  sensor.startMeasurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
  delay(100);
  Serial.print("measurements started\n");

  dustCharacteristic = BLEModule::createCharacteristic(dustUUID.c_str());
}

bool DustSensor::readSensorData()
{
  Wire.setClock(100000); // Sensor has max I2C freq of 1MHz

  uint16_t dataReadyFlag = 0;
  float mc1p0 = 0;
  float mc2p5 = 0;
  float mc4p0 = 0;
  float mc10p0 = 0;
  float nc0p5 = 0; // irrelevant for us
  float nc1p0 = 0; // irrelevant for us
  float nc2p5 = 0; // irrelevant for us
  float nc4p0 = 0; // irrelevant for us
  float nc10p0 = 0; // irrelevant for us
  float typicalParticleSize = 0; // irrelevant for us

  error = sensor.readDataReadyFlag(dataReadyFlag);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute readDataReadyFlag(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return false;
  }
  error = sensor.readMeasurementValuesFloat(mc1p0, mc2p5, mc4p0, mc10p0,
                                            nc0p5, nc1p0, nc2p5, nc4p0,
                                            nc10p0, typicalParticleSize);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute readMeasurementValuesFloat(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return false;
  }

  Serial.print("sps measured\n");

  if (measurementCallback)
  {
    measurementCallback({mc1p0, mc2p5, mc4p0, mc10p0});
  }

  if (sendBLE)
  {
    notifyBLE(mc1p0, mc2p5, mc4p0, mc10p0);
  }
  return true;
}

void DustSensor::notifyBLE(float pm1, float pm2_5, float pm4, float pm10)
{
  BLEModule::writeBLE(dustCharacteristic, pm1, pm2_5, pm4, pm10);
}