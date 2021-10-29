void initBMX()
{
    bmx.beginAcc(0x3);
    bmx.beginGyro();
};


void getAccAmplitudes(double *sumAccX, double *sumAccY, double *sumAccZ)
{

  // Acceleration Amplitude calc
  double amplitudeAccX = accX - bmx.getAccelerationX();
  double amplitudeAccY = accY - bmx.getAccelerationY();
  double amplitudeAccZ = accZ - bmx.getAccelerationZ();

  accX = bmx.getAccelerationX();
  accY = bmx.getAccelerationY();
  accZ = bmx.getAccelerationZ();
  *sumAccX += abs(amplitudeAccX);
  *sumAccY += abs(amplitudeAccY);
  *sumAccZ += abs(amplitudeAccZ);
};

