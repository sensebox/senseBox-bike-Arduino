/**
 * !!!IMPORTANT!!!WICHTIG!!!!
 * HIER NICHTS VERÄNDERN 
 * !!!IMPORTANT!!!WICHTIG!!!!
 */

#ifndef VARIABLES_H
#define VARIABLES_H

/// LIBRARIES
#include <senseBoxIO.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi101.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
#include <NewPing.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_HDC1000.h>
#include <SDS011-select-serial.h>
#include <avr/dtostrf.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <SDConfig.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
// Load other variable files
#include "ids.h"
// build id arrays
char *bmxIDS[] = {accXID, accYID, accZID};
char *distanceIDS[] = {distLeftID, distRightID};
char *sensorIDS[] = {tempID, humiID, pm10ID, pm25ID, speedID};
#include "network.h"
const char server[] PROGMEM = "ingress.opensensemap.org";
// non ssl-port osm
const int port = 80; 
/// DEFINES
#define TRIGGER_LEFT 1
#define ECHO_LEFT 2
#define TRIGGER_RIGHT 3
#define ECHO_RIGHT 4
#define SENSOR_MAX_RANGE 400
#define Addr_Accl 0x18
#define Addr_Gyro 0x68
#define Addr_Mag 0x10
// Object definitions
WiFiClient client;
Adafruit_HDC1000 HDC = Adafruit_HDC1000();
SDS011 SDS(Serial1);
SFE_UBLOX_GNSS myGNSS;
Adafruit_MPU6050 mpu;
// RGB LED
 Adafruit_NeoPixel rgb_led_6= Adafruit_NeoPixel(1, 6,NEO_GRB + NEO_KHZ800);
// Ultraschall
NewPing left(TRIGGER_LEFT, ECHO_LEFT, SENSOR_MAX_RANGE);
NewPing right(TRIGGER_RIGHT, ECHO_RIGHT, SENSOR_MAX_RANGE);



bool standby = false;
int standbycounter;


File myFile;


char *fileNames[] = {"bmx.csv", "sensor.csv", "distanz.csv"};

/// TIME VARIABLES
const long interval1s = 1000;
const long interval5s = 5000;
const long interval10s = 10000;
const long interval20s = 20000;
const long interval30s = 30000;
const long interval60s = 60000;
long timeoutLeft = 0;
long timeoutRight = 0;
long time_start = 0;
long time_start_standby = 0;
long time_actual_1s = 0;
long time_actual_5s = 0;
long time_actualstandby_5s = 0;
long time_actualstandby_60s = 0;
long time_actual_10s = 0;
long time_actual_30s = 0;
long time_actual_20s = 0;
long time_actual_60s = 0;
int standbyThresholdTime = 6;
double standbyThreshold = 2.0;
unsigned long previousMillis = 0;
unsigned long previousMillis10s = 0;


/// GLOBAL PLACEHOLDERS
char buffer[750];
char timestampGlobal[124];
char lngGlobal[20];
char latGlobal[20];
unsigned long clength = 0;
/// BMX
// SUMMED AMPLITUDES
double sumAccZ;
double sumAccY;
double sumAccX;
// Variables for BMX
double accX;
double accY;
double accZ;

// forward declarations
void initBMX(void);
void initUltrasonic(void);
void initGPS(void);
void initSD(void);
bool readConfiguration(char* confFile);
void dumpConfiguration(void);
bool isGPSvalid(void);
void getAccAmplitudes(double*, double*, double*);
void setTimestamp(void);
void setGPS(void);
void showGreen(void);
void handleLeft(void);
void handleRight(void);
void checkStandby(bool *standby);
void writeToSD(char *data, char *fileName);
void resetSD(void);
bool sdisempty(void);
void getMeasurements(void);
void connectToWifi(void);
bool submitValues(void);
void showRed(void);
void showBlue(void);
void showGreen(void);
#endif
