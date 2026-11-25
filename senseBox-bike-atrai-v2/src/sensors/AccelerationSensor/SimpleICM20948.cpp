#include "SimpleICM20948.h"

// Bank 0
#define REG_WHO_AM_I 0x00
#define REG_PWR_MGMT_1 0x06
#define REG_PWR_MGMT_2 0x07
#define REG_ACCEL_XOUT_H 0x2D
// Bank 2
#define REG_GYRO_SMPLRT_DIV 0x00
#define REG_GYRO_CONFIG_1 0x01
#define REG_ACCEL_SMPLRT_DIV_1 0x10
#define REG_ACCEL_SMPLRT_DIV_2 0x11
#define REG_ACCEL_CONFIG 0x14
// All banks
#define REG_BANK_SEL 0x7F

#define ICM20948_CHIP_ID 0xEA

bool SimpleICM20948::begin_I2C(uint8_t addr, TwoWire *wire)
{
  _addr = addr;
  _wire = wire;
  _wire->begin();

  setBank(0);
  if (readReg(REG_WHO_AM_I) != ICM20948_CHIP_ID)
    return false;

  writeReg(REG_PWR_MGMT_1, 0x80); // reset
  delay(100);
  setBank(0);
  writeReg(REG_PWR_MGMT_1, 0x01); // wake up, auto-select clock
  writeReg(REG_PWR_MGMT_2, 0x00); // enable accel and gyro
  delay(20);

  // Same defaults as Adafruit_ICM20948 after begin():
  // largest ranges, gyro ~100 Hz, accel ~53.6 Hz, low-pass filter on
  setBank(2);
  writeReg(REG_GYRO_CONFIG_1, (3 << 1) | 1); // +-2000 dps, DLPF on
  writeReg(REG_ACCEL_CONFIG, (3 << 1) | 1);  // +-16 g, DLPF on
  writeReg(REG_GYRO_SMPLRT_DIV, 10);
  setBank(0);
  _accelLsbPerG = 2048.0;
  _gyroLsbPerDps = 16.4;
  setAccelRateDivisor(20);

  return true;
}

void SimpleICM20948::setAccelRange(icm20948_accel_range_t range)
{
  setBank(2);
  writeReg(REG_ACCEL_CONFIG, ((uint8_t)range << 1) | 1); // keep DLPF on
  setBank(0);
  _accelLsbPerG = 16384.0 / (1 << range); // 16384, 8192, 4096, 2048
}

void SimpleICM20948::setAccelRateDivisor(uint16_t divisor)
{
  setBank(2);
  writeReg(REG_ACCEL_SMPLRT_DIV_1, (divisor >> 8) & 0x0F);
  writeReg(REG_ACCEL_SMPLRT_DIV_2, divisor & 0xFF);
  setBank(0);
}

bool SimpleICM20948::getEvent(sensors_event_t *accel, sensors_event_t *gyro,
                              sensors_event_t *temp, sensors_event_t *mag)
{
  // accel X/Y/Z, gyro X/Y/Z, temperature: 14 bytes from ACCEL_XOUT_H
  _wire->beginTransmission(_addr);
  _wire->write(REG_ACCEL_XOUT_H);
  if (_wire->endTransmission(false) != 0)
    return false;
  if (_wire->requestFrom(_addr, (uint8_t)14) != 14)
    return false;

  int16_t raw[7];
  for (int i = 0; i < 7; i++)
  {
    uint8_t hi = _wire->read();
    uint8_t lo = _wire->read();
    raw[i] = (int16_t)((hi << 8) | lo);
  }

  uint32_t t = millis();

  if (accel)
  {
    memset(accel, 0, sizeof(sensors_event_t));
    accel->version = sizeof(sensors_event_t);
    accel->type = SENSOR_TYPE_ACCELEROMETER;
    accel->timestamp = t;
    accel->acceleration.x = raw[0] / _accelLsbPerG * SENSORS_GRAVITY_EARTH;
    accel->acceleration.y = raw[1] / _accelLsbPerG * SENSORS_GRAVITY_EARTH;
    accel->acceleration.z = raw[2] / _accelLsbPerG * SENSORS_GRAVITY_EARTH;
  }
  if (gyro)
  {
    memset(gyro, 0, sizeof(sensors_event_t));
    gyro->version = sizeof(sensors_event_t);
    gyro->type = SENSOR_TYPE_GYROSCOPE;
    gyro->timestamp = t;
    gyro->gyro.x = raw[3] / _gyroLsbPerDps * SENSORS_DPS_TO_RADS;
    gyro->gyro.y = raw[4] / _gyroLsbPerDps * SENSORS_DPS_TO_RADS;
    gyro->gyro.z = raw[5] / _gyroLsbPerDps * SENSORS_DPS_TO_RADS;
  }
  if (temp)
  {
    memset(temp, 0, sizeof(sensors_event_t));
    temp->version = sizeof(sensors_event_t);
    temp->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
    temp->timestamp = t;
    temp->temperature = (raw[6] - 21) / 333.87 + 21.0;
  }
  if (mag)
  {
    memset(mag, 0, sizeof(sensors_event_t)); // no magnetometer
    mag->version = sizeof(sensors_event_t);
    mag->type = SENSOR_TYPE_MAGNETIC_FIELD;
    mag->timestamp = t;
  }
  return true;
}

void SimpleICM20948::setBank(uint8_t bank)
{
  writeReg(REG_BANK_SEL, (bank & 0x03) << 4);
}

void SimpleICM20948::writeReg(uint8_t reg, uint8_t val)
{
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(val);
  _wire->endTransmission();
}

uint8_t SimpleICM20948::readReg(uint8_t reg)
{
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom(_addr, (uint8_t)1);
  return _wire->available() ? _wire->read() : 0;
}
