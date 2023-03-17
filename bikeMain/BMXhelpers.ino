
void initBMX()
{
  // Try to initialize!
  if (!mpu.begin()) {
  #ifdef DEBUG_ENABLED
  Serial.println("Failed to find MPU6050 chip");
  #endif
    while (1) {
      showRed();
      delay(10);
    }
  }
  #ifdef DEBUG_ENABLED
  Serial.println("MPU6050 Found!");
  #endif
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  #ifdef DEBUG_ENABLED
  Serial.print("Accelerometer range set to: ");
  #endif
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
  #ifdef DEBUG_ENABLED
   Serial.println("+-2G");
  #endif
    break;
  case MPU6050_RANGE_4_G:
  #ifdef DEBUG_ENABLED
  Serial.println("+-4G");
  #endif
    break;
  case MPU6050_RANGE_8_G:
  #ifdef DEBUG_ENABLED
  Serial.println("+-8G");
  #endif
    break;
  case MPU6050_RANGE_16_G:
  #ifdef DEBUG_ENABLED
  Serial.println("+-16G");
  #endif
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  #ifdef DEBUG_ENABLED
  Serial.print("Gyro range set to: ");
  #endif
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
  #ifdef DEBUG_ENABLED
    Serial.println("+- 250 deg/s");
  #endif
    break;
  case MPU6050_RANGE_500_DEG:
  #ifdef DEBUG_ENABLED
    Serial.println("+- 500 deg/s");
  #endif
    break;
  case MPU6050_RANGE_1000_DEG:
  #ifdef DEBUG_ENABLED
    Serial.println("+- 1000 deg/s");
  #endif
    break;
  case MPU6050_RANGE_2000_DEG:
  #ifdef DEBUG_ENABLED
    Serial.println("+- 2000 deg/s");
  #endif
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  #ifdef DEBUG_ENABLED
  Serial.print("Filter bandwidth set to: ");
  #endif

  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
  #ifdef DEBUG_ENABLED
    Serial.println("260 Hz");
  #endif
    break;
  case MPU6050_BAND_184_HZ:
  #ifdef DEBUG_ENABLED
    Serial.println("184 Hz");
  #endif
    break;
  case MPU6050_BAND_94_HZ:
  #ifdef DEBUG_ENABLED
    Serial.println("94 Hz");
  #endif
    break;
  case MPU6050_BAND_44_HZ:
  #ifdef DEBUG_ENABLED
    Serial.println("44 Hz");
  #endif
    break;
  case MPU6050_BAND_21_HZ:
  #ifdef DEBUG_ENABLED
    Serial.println("21 Hz");
  #endif
    break;
  case MPU6050_BAND_10_HZ:
  #ifdef DEBUG_ENABLED
    Serial.println("10 Hz");
  #endif
    break;
  case MPU6050_BAND_5_HZ:
  #ifdef DEBUG_ENABLED
    Serial.println("5 Hz");
  #endif
    break;
  }
  delay(100);
  
};


void getAccAmplitudes(float *sumAccX, float *sumAccY, float *sumAccZ)
{

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // Acceleration Amplitude calc

  float amplitudeAccX = accX - a.acceleration.x;
  float amplitudeAccY = accY - a.acceleration.y;
  float amplitudeAccZ = accZ - a.acceleration.z;

  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;

  if(abs(amplitudeAccX) > 0.05)
  {
     *sumAccX += abs(amplitudeAccX);
  }
  
  if(abs(amplitudeAccY) > 0.05)
  {
  *sumAccY += abs(amplitudeAccY);
  }
  
  if(abs(amplitudeAccZ) > 0.05)
  {
  *sumAccZ += abs(amplitudeAccZ);
  }
  
};
