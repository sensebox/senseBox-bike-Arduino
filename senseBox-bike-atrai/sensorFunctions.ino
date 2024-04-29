float getVl53l8cxMin() {

  VL53L8CX_ResultsData Results;
  uint8_t NewDataReady = 0;
  uint8_t status;

  float oldVl53l8cxMin = -1.0;
  status = sensor_vl53l8cx_top.vl53l8cx_check_data_ready(&NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    sensor_vl53l8cx_top.vl53l8cx_get_ranging_data(&Results);
    float min = 10000.0;
    for (int i = 0; i < VL53L8CX_RESOLUTION_8X8 * VL53L8CX_NB_TARGET_PER_ZONE; i++) {
      if ((&Results)->target_status[i] != 255) {
        float distance = (&Results)->distance_mm[i];
        if (min > distance) {
          min = distance;
        }
      }
    }
    oldVl53l8cxMin = (min == 10000.0) ? 0.0 : min;
  }
  return oldVl53l8cxMin;
}

void callSPS() {
  //struct sps30_measurement m;
  char serial[SPS30_MAX_SERIAL_LEN];
  uint16_t data_ready;
  int16_t ret;
  do {
    ret = sps30_read_data_ready(&data_ready);
    if (ret < 0) {
      Serial.print("error reading data-ready flag: ");
      Serial.println(ret);
    } else if (!data_ready)
      Serial.print("data not ready, no new measurement available\n");
    else
      break;
    delay(100); /* retry in 100ms */
  } while (1);

  ret = sps30_read_measurement(&m);
  if (ret < 0) {
    Serial.print("error reading measurement\n");
  }
}



void getAccAmplitudes(float *sumAccX, float *sumAccY, float *sumAccZ) {

  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);
  // Acceleration Amplitude calc

  float amplitudeAccX = accX - a.acceleration.x;
  float amplitudeAccY = accY - a.acceleration.y;
  float amplitudeAccZ = accZ - a.acceleration.z;

  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;

  if (abs(amplitudeAccX) > 0.05) {
    *sumAccX += abs(amplitudeAccX);
  }

  if (abs(amplitudeAccY) > 0.05) {
    *sumAccY += abs(amplitudeAccY);
  }

  if (abs(amplitudeAccZ) > 0.05) {
    *sumAccZ += abs(amplitudeAccZ);
  }
};
