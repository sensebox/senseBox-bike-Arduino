void initBMX() {
  // Try to initialize!
  if (!mpu.begin(0x68, &Wire1)) {
    Serial.println("MPU6050 Chip wurde nicht gefunden");
    return;
  }
 else {
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    delay(100);
  };
}

void initUltrasonic() {
  //pinmodes for ultrasonic
  pinMode(TRIGGER_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  // pinMode(TRIGGER_RIGHT, OUTPUT);
  // pinMode(ECHO_RIGHT, INPUT);
};

void initVL53L8CX() {
  Wire.begin();
  Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz
  sensor_vl53l8cx_top.begin();
  sensor_vl53l8cx_top.init_sensor();
  sensor_vl53l8cx_top.vl53l8cx_set_ranging_frequency_hz(30);
  sensor_vl53l8cx_top.vl53l8cx_set_resolution(VL53L8CX_RESOLUTION_8X8);
  sensor_vl53l8cx_top.vl53l8cx_start_ranging();
}

void initSPS() {
  int errorCount = 0;
  int16_t ret;
  uint8_t auto_clean_days = 4;
  uint32_t auto_clean;

  delay(2000);

  sensirion_i2c_init();
  while (sps30_probe() != 0) {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
  }
  ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
  if (ret) {
    Serial.print("error setting the auto-clean interval: ");
    Serial.println(ret);
  }

  ret = sps30_start_measurement();
  if (ret < 0) {
    Serial.print("error starting measurement\n");
  }
  Serial.println("Starting SPS Measurements");
}

void initGPS() {
  Serial1.begin(9600);
};
