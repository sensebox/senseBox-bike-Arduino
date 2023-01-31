#include "variables/variables.h"

#define DEBUG_ENABLED




void setup()
{

#ifdef DEBUG_ENABLED
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Starting");
#endif

  senseBoxIO.powerNone();
  delay(1000);
  senseBoxIO.powerAll();
  delay(100);

  Serial.println("");
  delay(100);



  HDC.begin();
  Serial1.begin(9600);
  initUltrasonic();
  // init sps
  initSPS();
  initSD();
#ifdef DEBUG_ENABLED
  Serial.print("try to read configuration from ");
  Serial.println(confFile);
#endif
  // bool ret = readConfiguration(confFile);
#ifdef DEBUG_ENABLED
  // if (ret) {
  //   Serial.println("OK");
  // } else {
  //   Serial.println("not found");
  // }
  // dumpConfiguration();
#endif


  //checkForFiles();
  //resetSD();
  rgb_led_1.begin();
  rgb_led_1.setBrightness(30);

  initBMX();
}

void loop()
{
  time_start = millis();
  while (gps.available( Serial1 )) {
    fix = gps.read();
  }
  if (!standby)
  {
    if (fix.valid.location)
    {
      getAccAmplitudes(&sumAccX, &sumAccY, &sumAccZ);
      setTimestamp();
      setGPS();
      showGreen();
      // if a new distance value came in save to sd
      // handleLeft();
      // handleRight();
      // check for standby every 10 seconds
      if (time_start > time_actual_10s + interval10s)
      {
        checkStandby(&standby);
        time_actual_10s = millis();
      }
      if (time_start > time_actual_20s + interval20s)
      {
        getMeasurements();
        time_actual_20s = millis();
      }
    }
    // Give debug message for fix type and show rgb led in different states
    else //!GPS
    {
      showRed();
#ifdef DEBUG_ENABLED
#endif
      if (time_start > time_actual_60s + interval60s)
      {
        time_actual_60s = millis();
#ifdef DEBUG_ENABLED
        Serial.println("60s passed");
#endif
        if (!sdisempty()) {
          connectToWifiWrapper();
        }
        if (WiFi.status() == WL_CONNECTED)
        {
          // check for files on sd ?
          if (submitValues()) // success; if http gets 200 back go to sleep
          {
#ifdef DEBUG_ENABLED
            // Uploaded all values to home wifi, goes to sleep afterwards
            Serial.println("Going to hibernation");
#endif
            showRed();
            for (;;)
            {
              WiFi.disconnect();
              senseBoxIO.powerNone();
            }
          }
        }
      }
      // connect to wifi
    }
  }
  // start different mode with time intervals
  // Checks every 10 seconds if the standby status has changed
  // Trys every 60 seconds to connect to saved WiFi and upload the values
  else // !standby
  {
    showBlue();
    getAccAmplitudes(&sumAccX, &sumAccY, &sumAccZ);
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis10s >= interval1s)
    {
      previousMillis10s = currentMillis;
#ifdef DEBUG_ENABLED
      Serial.println("Checking standby");
#endif
      checkStandby(&standby);
    }
    if (currentMillis - previousMillis >= interval60s)
    {
#ifdef DEBUG_ENABLED
      Serial.println("Connecting to wifi");
#endif
      previousMillis = currentMillis;
      connectToWifiWrapper();
     
      if (WiFi.status() == WL_CONNECTED)
      {
        if (submitValues()) // success; if http gets 200 back go to sleep
        {
#ifdef DEBUG_ENABLED
          // Uploaded all values to home wifi, goes to sleep afterwards
          Serial.println("Going to hibernation");
#endif
          showRed();
          for (;;)
          {
            WiFi.disconnect();
            senseBoxIO.powerNone();
          }
        }
        // disconnect and try again in 60 seconds
        WiFi.disconnect();
      }
    }
  }
}
