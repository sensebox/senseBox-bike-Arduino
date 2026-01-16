void handleDistance() {
    dist_l = sonarA.ping_cm();
    if (dist_l == 0) {
      dist_l = 400;
    }
  // if (dist_l < 150) {
  //   if (!recording) {
  //     if (currentTime - last > 1000) {
  //       Serial.print("Ueberholmanöver entdeckt: ");
  //       Serial.println(dist_l);
  //       last = millis();
  //       recording = true;
  //       SenseBoxBLE::write(distanceCharacteristic, dist_l);

  //       Serial.print(dist_l);
  //     }
  //   }
  // }
  // if (dist_l > 150 && recording) {
  //   if (recording) {
  //     Serial.print("Ueberholmanoever vorbei: ");
  //     Serial.println(dist_l);
  //   }
  //   recording = false;
  // }
}

/*  To give a confidence rating, a target with status 5 is considered as 100% valid. 
A status of 6 or 9 can be considered with a confidence value of 50%. 
All other statuses are below the 50% confidence level. */
bool validTargetStatus(int status) {
  return status == 5 || status == 6 || status == 9;
}

void handleDistanceVL53L8CX() {
  VL53L8CX_ResultsData Results;
  uint8_t NewDataReady = 0;
  uint8_t status;

  status = sensor_vl53l8cx_top.vl53l8cx_check_data_ready(&NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    sensor_vl53l8cx_top.vl53l8cx_get_ranging_data(&Results);
    float min = 1000.0; // larger than what the sensor could possibly see
    for(int i = 0; i < VL53L8CX_RESOLUTION_8X8*VL53L8CX_NB_TARGET_PER_ZONE; i++) {
      if(validTargetStatus((int)(&Results)->target_status[i])){
        float distance = ((&Results)->distance_mm[i])/10;
        if(min > distance) {
          min = distance;
        }
      }
    }
    dist_l = (min==1000.0) ? 400.0 : min; // in theory the sensor can measure up to 4m distance
  }
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
