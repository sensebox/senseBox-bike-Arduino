void initBMX()
{
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
};


void getAccAmplitudes(double *sumAccX, double *sumAccY, double *sumAccZ)
{

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // Acceleration Amplitude calc
  double amplitudeAccX = accX - a.acceleration.x;
  double amplitudeAccY = accY - a.acceleration.y;
  double amplitudeAccZ = accZ - a.acceleration.z;

  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;
  *sumAccX += abs(amplitudeAccX);
  *sumAccY += abs(amplitudeAccY);
  *sumAccZ += abs(amplitudeAccZ);
};

