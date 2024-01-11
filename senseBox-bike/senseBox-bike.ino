#include <SenseBoxMCU.h>
#include "SenseBoxBLE.h"
#include <Wire.h>
#include <SDConfig.h>
#include <SD.h>
#include <Adafruit_HDC1000.h>
#include <sps30.h>
#include <NMEAGPS.h>
#include <Adafruit_MPU6050.h>
#include <NewPing.h>  // http://librarymanager/All#NewPing

#define TRIGGER_LEFT 1
#define ECHO_LEFT 2
#define MAX_DISTANCE_A 400

#define Addr_Accl 0x18
#define Addr_Gyro 0x68
#define Addr_Mag 0x10
NewPing sonarA(TRIGGER_LEFT, ECHO_LEFT, MAX_DISTANCE_A);

Adafruit_MPU6050 mpu;
Adafruit_HDC1000 HDC = Adafruit_HDC1000();
static NMEAGPS gps;
// https://github.com/SlashDevin/NeoGPS/blob/master/extras/doc/Data%20Model.md
static gps_fix fix;

int period = 1000;
unsigned long time_now = 0;
bool recording = false;
long last;
long time_start = 0;
unsigned long previousMillis = 0;  // Variable zur Speicherung der letzten Millisekunden

float temp = 0;
float humi = 0;
float pm1 = 0;
float pm25 = 0;
float pm4 = 0;
float pm10 = 0;
float accX = 0;
float accY = 0;
float accZ = 0;
float gps_lat = 0;
float gps_lng = 0;
float gps_spd = 0;
float dist_l = 0;
struct sps30_measurement m;
float speed;
float latitude;
float longitude;
float sumAccZ;
float sumAccY;
float sumAccX;

int temperatureCharacteristic = 0;
int humidityCharacteristic = 0;
int PMCharacteristic = 0;
int accelerationCharacteristic = 0;
int GPSCharacteristic = 0;
int distanceCharacteristic = 0;
int GPSFixCharacteristisc = 0;
float status;
String name;



void initSensors() {
  Serial.print("Ultrasonic...");
  initUltrasonic();
  Serial.println("done!");
  Serial.print("SPS30...");
  initSPS();
  Serial.println("done!");
  Serial.print("MPU6050...");
  initBMX();
  Serial.println("done!");
  Serial.print("GPS...");
  initGPS();
  Serial.println("done!");
  Serial.print("HDC1080...");
  if (!HDC.begin()) {
    Serial.println("Couldn't find HDC1080!");
  }
  Serial.println("done!");
  Serial.print("Propeller...");
  pinMode(3, OUTPUT);
  delay(100);
  digitalWrite(3, HIGH);
  Serial.println("done!");
}



void setMeasurements() {
  getAccAmplitudes(&sumAccX, &sumAccY, &sumAccZ);

  handleDistance();
  temp = HDC.readTemperature();
  humi = HDC.readHumidity();

  callSPS();

  pm10 = m.mc_10p0;
  pm25 = m.mc_2p5;
  pm4 = m.mc_4p0;
  pm1 = m.mc_1p0;
}

// Helper function to bypass the delay function 
void smartDelay (unsigned int milliseconds){
  unsigned long currentMillis = millis();
  unsigned long interval = milliseconds;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  } 
}

void startBluetooth () {
  SenseBoxBLE::start("senseBox-BLE");
  smartDelay(1000);
  name = "senseBox:bike [" + SenseBoxBLE::getMCUId() + "]";
  SenseBoxBLE::setName(name);
  Serial.println(name);
  smartDelay(1000);
  Serial.print("Adding BLE characteristics...");
  SenseBoxBLE::addService("CF06A218F68EE0BEAD048EBC1EB0BC84");
  temperatureCharacteristic = SenseBoxBLE::addCharacteristic("2CDF217435BEFDC44CA26FD173F8B3A8");
  humidityCharacteristic = SenseBoxBLE::addCharacteristic("772DF7EC8CDC4EA986AF410ABE0BA257");
  PMCharacteristic = SenseBoxBLE::addCharacteristic("7E14E07084EA489FB45AE1317364B979");
  accelerationCharacteristic = SenseBoxBLE::addCharacteristic("B944AF10F4954560968F2F0D18CAB522");
  GPSCharacteristic = SenseBoxBLE::addCharacteristic("8EDF8EBB12464329928DEE0C91DB2389");
  distanceCharacteristic = SenseBoxBLE::addCharacteristic("B3491B60C0F34306A30D49C91F37A62B");
  GPSFixCharacteristisc = SenseBoxBLE::addCharacteristic("PPFSSPSPXGIPIIPFFXXPSXSFSSXSPXFF");
  Serial.println("done!");

}

// sends phenomenas to the given BT characteristics
void writeToBluetooth(){
  bool connected = SenseBoxBLE::write(temperatureCharacteristic, temp);
  SenseBoxBLE::write(humidityCharacteristic, humi);
  SenseBoxBLE::write(PMCharacteristic, pm1, pm25, pm4, pm10);
  SenseBoxBLE::write(accelerationCharacteristic, sumAccX, sumAccY, sumAccZ);
  SenseBoxBLE::write(GPSCharacteristic, latitude, longitude, speed);
  SenseBoxBLE::write(distanceCharacteristic, dist_l);
  SenseBoxBLE::write(GPSFixCharacteristisc, status);

}

// sets global variables for gps
void setGPSValues(){
  speed = (float)fix.speed_kph();
  gps_fix::status_t statusTmp = fix.status; 
  switch (statusTmp) {
    case gps_fix::STATUS_NONE: 
      status = 0;
      break;
    case gps_fix::STATUS_EST: 
      status = 1; 
      break; 
    case gps_fix::STATUS_TIME_ONLY: 
      status = 2; 
      break; 
    case gps_fix::STATUS_DGPS: 
      status = 3;
      break;
    default: 
      status = 0;
      break;
  }
  latitude = (float)fix.latitude();
  longitude = (float)fix.longitude();
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Sketch!");


  Serial.println("Initializing sensors..");
  initSensors();
  Serial.println("Sensor init done!");

  smartDelay(10000);
  startBluetooth();
  smartDelay(500);

}

void loop() {
  SenseBoxBLE::poll();
  time_start = millis();
  while (gps.available(Serial1)) {
    fix = gps.read();
  }

  setMeasurements();
  setGPSValues();
  writeToBluetooth();
  sumAccX = 0;
  sumAccY = 0;
  sumAccZ = 0;

  Serial.println(name);


  time_now = millis();

  while (millis() < time_start + period) {
    SenseBoxBLE::poll();
    smartDelay(5);
  }
}