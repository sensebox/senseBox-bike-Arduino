#include "variables/variables.h"

//#define DEBUG_ENABLED

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
  initBMX();
  HDC.begin();
  Serial1.begin(9600);
  initUltrasonic();
  initGPS();
  initSD();
  //checkForFiles();
  //resetSD();
  rgb_led_1.begin();
  rgb_led_1.setBrightness(30);
}

void loop()
{
  time_start = millis();
  if (!standby)
  {
    if (isGPSvalid())
    {
      getAccAmplitudes(&sumAccX, &sumAccY, &sumAccZ);
      setTimestamp();
      setGPS();
      showGreen();
      // if a new distance value came in save to sd
      handleLeft();
      handleRight();
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
      if (time_start > time_actual_60s + interval60s)
      {
        time_actual_60s = millis();
#ifdef DEBUG_ENABLED
        Serial.println("60s passed Looking for wifi");
#endif
        if (!sdisempty()) {
          connectToWifi();
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
      // let led show yellow
      byte fixType = myGNSS.getFixType();
      if (fixType == 0)
      {
        showRed();
#ifdef DEBUG_ENABLED
        Serial.println(F("No fix"));
#endif
      }
      else if (fixType == 1)
      {
        showRed();
#ifdef DEBUG_ENABLED
        Serial.println(F("Dead reckoning"));
#endif
      }
      else if (fixType == 3)
      {
#ifdef DEBUG_ENABLED
        Serial.println(F("3D"));
#endif
      }
      else if (fixType == 4)
      {
        showRed();
#ifdef DEBUG_ENABLED
        Serial.println(F("GNSS + Dead reckoning"));
#endif
      }
      else if (fixType == 5)
      {
        showRed();
#ifdef DEBUG_ENABLED
        Serial.println(F("Time only"));
#endif
      }
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
      connectToWifi();
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
