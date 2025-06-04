#include "Arduino.h"
#include <Wire.h>
#include <sps30.h>

void init_sps30();
bool read_sps30(float& pm1_0, float& pm2_5, float& pm4_0, float& pm10);