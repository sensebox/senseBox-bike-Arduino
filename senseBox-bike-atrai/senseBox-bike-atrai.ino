// First sketch for the senseBox:bike working with the second generation for the MCU

#include <SenseBoxBLE.h>
#include <Adafruit_HDC1000.h>
#include <sps30.h>
#include <Adafruit_MPU6050.h>
#include <NewPing.h>  // http://librarymanager/All#NewPing
#include "display.h"

#include <vl53l8cx_class.h>




// Accelerometer and Gyroscope
Adafruit_MPU6050 mpu;
// Temperatur and humidity
Adafruit_HDC1000 HDC = Adafruit_HDC1000();
// ToF
VL53L8CX sensor_vl53l8cx_top(&Wire, -1, -1);

int period = 1000;
unsigned long time_now = 0;
bool recording = false;
long last;
long time_start = 0;

float temp = 0;
float humi = 0;
float pm1 = 0;
float pm25 = 0;
float pm4 = 0;
float pm10 = 0;
float accX = 0;
float accY = 0;
float accZ = 0;
float distance = 0;
struct sps30_measurement m;


float sumAccZ;
float sumAccY;
float sumAccX;

int temperatureCharacteristic = 0;
int humidityCharacteristic = 0;
int PMCharacteristic = 0;
int accelerationCharacteristic = 0;
int distanceCharacteristic = 0;
float status;

String name;

// init all sensors
// todo: give feedback through LED if all is working?
void initsSensors() {
  pinMode(IO_ENABLE, OUTPUT);
  digitalWrite(IO_ENABLE, LOW);
  delay(1000);
  Serial.print("ToF...");
  SBDisplay::showLoading("Init ToF...", 0.2);
  initToF();
  // ATTENTION! SPS Disabled for essen-auf-raedern
  Serial.print("SPS30...");
  SBDisplay::showLoading("Init SPS30...", 0.3);
  initSPS();
  Serial.println("done!");
  Serial.print("MPU6050...");
  SBDisplay::showLoading("Init MPU6050...", 0.4);
  initBMX();
  Serial.println("done!");
  Serial.print("HDC1080...");
  SBDisplay::showLoading("Init HDC1080...", 0.5);

  if (!HDC.begin()) {
    Serial.println("Couldn't find HDC1080!");
  }
  Serial.println("done!");
  Serial.print("Propeller...");
  SBDisplay::showLoading("Ventilation...", 0.6);
  pinMode(3, OUTPUT);
  delay(100);
  digitalWrite(3, HIGH);
  SBDisplay::showLoading("Sensors done...", 0.7);
  Serial.println("done!");
}

// set measurements for acceleration, distance, humidity and temperature
void setMeasurements() {
  getAccAmplitudes(&sumAccX, &sumAccY, &sumAccZ);
  temp = HDC.readTemperature();
  humi = HDC.readHumidity();
  callSPS();

  pm10 = m.mc_10p0;
  pm25 = m.mc_2p5;
  pm4 = m.mc_4p0;
  pm1 = m.mc_1p0;

  distance = getVl53l8cxMin();
}

// starts bluetooth and sets the name according to the Bluetooth Bee
// TODO: can the ID be seen on the hardware?
void startBluetooth() {
  Serial.print("Set up Bluetooth");
  SenseBoxBLE::start("senseBox-BLE");
  delay(1000);
  name = "senseBox:bike [" + SenseBoxBLE::getMCUId() + "]";
  SenseBoxBLE::setName(name);
  Serial.println(name);
  delay(1000);
  Serial.print("Adding BLE characteristics...");
  SenseBoxBLE::addService("CF06A218F68EE0BEAD048EBC1EB0BC84");
  temperatureCharacteristic = SenseBoxBLE::addCharacteristic("2CDF217435BEFDC44CA26FD173F8B3A8");
  humidityCharacteristic = SenseBoxBLE::addCharacteristic("772DF7EC8CDC4EA986AF410ABE0BA257");
  PMCharacteristic = SenseBoxBLE::addCharacteristic("7E14E07084EA489FB45AE1317364B979");
  accelerationCharacteristic = SenseBoxBLE::addCharacteristic("B944AF10F4954560968F2F0D18CAB522");
  distanceCharacteristic = SenseBoxBLE::addCharacteristic("B3491B60C0F34306A30D49C91F37A62B");
  Serial.println("done!");
}

// sends phenomenas to the given BT characteristics
bool writeToBluetooth() {
  bool connected = SenseBoxBLE::write(temperatureCharacteristic, temp);
  SenseBoxBLE::write(humidityCharacteristic, humi);
  SenseBoxBLE::write(PMCharacteristic, pm1, pm25, pm4, pm10);
  SenseBoxBLE::write(accelerationCharacteristic, sumAccX, sumAccY, sumAccZ);
  SenseBoxBLE::write(distanceCharacteristic, distance);

  return connected;
}

void resetVariables() {
  sumAccX = 0;
  sumAccY = 0;
  sumAccZ = 0;
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Starting Sketch!");
  SBDisplay::begin();
  Serial.println("Initializing sensors..");
  SBDisplay::showLoading("Init Sensors...", 0.1);
  initsSensors();
  Serial.println("Sensor init done! ");
  SBDisplay::showLoading("Setup BLE...", 0.8);
  startBluetooth();
  SBDisplay::showLoading("BLE done...", 0.9);
  Serial.println("Bluetooth done!");
  delay(500);
  SBDisplay::showLoading("Start measurements...", 1);
}

void loop() {
  SenseBoxBLE::poll();
  time_start = millis();

  // set global variables for measurements
  setMeasurements();
  // write measurements to bluetooth
  bool isConnected = writeToBluetooth();
  // reset acceleration values
  resetVariables();

  if (!isConnected) {
    String bleId = "[" + SenseBoxBLE::getMCUId() + "]";
    String bleIdBegin = bleId.substring(0, bleId.length() / 2);
    String bleIdEnd = bleId.substring(bleId.length() / 2);
    const char *MESSAGE_CONFIGURE_WIFI[4] = {
      "senseBox",
      "bike",
      bleIdBegin.c_str(),
      bleIdEnd.c_str()
    };
    SBDisplay::drawQrCode(name.c_str(), MESSAGE_CONFIGURE_WIFI);
  } else {
    SBDisplay::showSystemStatus();
  }

  time_now = millis();
  while (millis() < time_start + period) {
    SenseBoxBLE::poll();
    delay(5);
  }
}