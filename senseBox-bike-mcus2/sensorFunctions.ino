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
