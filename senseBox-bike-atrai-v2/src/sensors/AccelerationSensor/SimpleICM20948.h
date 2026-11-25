#ifndef SIMPLE_ICM20948_H
#define SIMPLE_ICM20948_H

// Minimal ICM-20948 driver: accelerometer, gyroscope and temperature only.
// Unlike Adafruit_ICM20948, it does not set up the magnetometer (AK09916),
// so it also works on boards where the magnetometer does not respond.
// The API mirrors the parts of Adafruit_ICM20948 we use.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>

typedef enum
{
  ICM20948_ACCEL_RANGE_2_G,
  ICM20948_ACCEL_RANGE_4_G,
  ICM20948_ACCEL_RANGE_8_G,
  ICM20948_ACCEL_RANGE_16_G,
} icm20948_accel_range_t;

class SimpleICM20948
{
public:
  bool begin_I2C(uint8_t addr = 0x69, TwoWire *wire = &Wire);
  void setAccelRange(icm20948_accel_range_t range);
  void setAccelRateDivisor(uint16_t divisor);
  // Same argument order as Adafruit: accel, gyro, temp, mag. mag is left empty.
  bool getEvent(sensors_event_t *accel, sensors_event_t *gyro,
                sensors_event_t *temp, sensors_event_t *mag = nullptr);

private:
  TwoWire *_wire = nullptr;
  uint8_t _addr = 0x68;
  float _accelLsbPerG = 2048.0;   // +-16 g
  float _gyroLsbPerDps = 16.4;    // +-2000 dps

  void setBank(uint8_t bank);
  void writeReg(uint8_t reg, uint8_t val);
  uint8_t readReg(uint8_t reg);
};

#endif // SIMPLE_ICM20948_H
