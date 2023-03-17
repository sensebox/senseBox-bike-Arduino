void resetVariables()
{
  sumAccZ = 0;
  sumAccY = 0;
  sumAccX = 0;
}

void initSPS()
{
  int16_t ret;
  uint8_t auto_clean_days = 4;
  uint32_t auto_clean;
  sensirion_i2c_init();
    while (sps30_probe() != 0) {
    #ifdef DEBUG_ENABLED
    Serial.print("SPS sensor probing failed\n");
    #endif
    delay(500);
  }
    ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
  if (ret) {
    #ifdef DEBUG_ENABLED
    Serial.print("error setting the auto-clean interval: ");
    Serial.println(ret);
    #endif

  }
  
  ret = sps30_start_measurement();
  if (ret < 0) {
    #ifdef DEBUG_ENABLED
    Serial.print("error starting measurement\n");
    #endif
  }
}

uint8_t connectToWifiWrapper(){
  uint8_t status;

  for(int i = 0; i < (sizeof(ssids) / sizeof(ssids[0])); i++)
  { 
    if(WiFi.status() != WL_CONNECTED) {
      #ifdef DEBUG_ENABLED
      Serial.print("Now trying to connect to: ");
      Serial.println(ssids[i]);
      #endif
      status = connectToWifi(ssids[i], pws[i]);    }

  }

}

uint8_t connectToWifi(char *ssid, char *pw)
{
  // Check WiFi Bee status
  if (WiFi.status() == WL_NO_SHIELD)
  {
#ifdef DEBUG_ENABLED
    Serial.println(F("WiFi shield not present"));
#endif

    // don't continue:
    while (true)
      ;
  }
  uint8_t status = WL_IDLE_STATUS;
  // attempt to connect to Wifi network:
  // Connect to WPA/WPA2 network. Change this line if using open or WEP
  // network
  status = WiFi.begin(ssid, pw);
  // wait 10 seconds for connection:
#ifdef DEBUG_ENABLED
  Serial.print(F("Waiting 10 seconds for connection..."));
#endif
  delay(10000);
#ifdef DEBUG_ENABLED
  Serial.println(F("done."));
#endif
  return status;
}

void getMeasurements()
{
  struct sps30_measurement m;
  char serial[SPS30_MAX_SERIAL_LEN];
  uint16_t data_ready;
  int16_t ret;
    do {
    ret = sps30_read_data_ready(&data_ready);
    if (ret < 0) {
      #ifdef DEBUG_ENABLED
      Serial.print("error reading data-ready flag: ");
      Serial.println(ret);
      #endif
    } 
    #ifdef DEBUG_ENABLED
     else if (!data_ready){
      Serial.print("data not ready, no new measurement available\n");
      }
     #endif

    else
      break;
    delay(100); /* retry in 100ms */
  } while (1);
    ret = sps30_read_measurement(&m);
  if (ret < 0) {
    #ifdef DBEUG_ENABLED
    Serial.print("error reading measurement\n");
    #endif
  } 

  float bmxValues[] = {sumAccX, sumAccY , sumAccZ}  ;

  float speed = fix.speed_kph();
  float sensorValues[5] = {HDC.readTemperature(), HDC.readHumidity(), m.mc_10p0, m.mc_2p5, speed};
  float temp = HDC.readTemperature();
  float humi = HDC.readHumidity();
  float pm10 = m.mc_10p0;
  float pm25 = m.mc_2p5; 
  float pm4 = m.mc_4p0; 
  float pm1 = m.mc_1p0;

  #ifdef DEBUG_ENABLED
  Serial.println("Sensor measurements");
  #endif   
  char buffer[750];

  sprintf_P(buffer, PSTR("%s,%.2f,%s,%02s,%02s\r\n\0"), tempID, temp, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
  Serial.print(buffer);
#endif
  writeToSD(buffer, "hdc.csv");

    sprintf_P(buffer, PSTR("%s,%.2f,%s,%02s,%02s\r\n\0"), humiID, humi, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
  Serial.print(buffer);
#endif
  writeToSD(buffer, "hdc.csv");



  sprintf_P(buffer, PSTR("%s,%.0f,%s,%02s,%02s\r\n\0"), pm10ID, pm10, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
  Serial.print(buffer);
#endif
  writeToSD(buffer, "sps.csv");

  sprintf_P(buffer, PSTR("%s,%.0f,%s,%02s,%02s\r\n\0"), pm25ID, pm25, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
  Serial.print(buffer);
#endif
  writeToSD(buffer, "sps.csv");

    sprintf_P(buffer, PSTR("%s,%.0f,%s,%02s,%02s\r\n\0"), pm4ID, pm4, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
  Serial.print(buffer);
#endif
  writeToSD(buffer, "sps2.csv");

    sprintf_P(buffer, PSTR("%s,%.0f,%s,%02s,%02s\r\n\0"), pm1ID, pm1, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
  Serial.print(buffer);
#endif
  writeToSD(buffer, "sps2.csv");

  sprintf_P(buffer, PSTR("%s,%.1f,%s,%02s,%02s\r\n\0"), speedID, speed, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
  Serial.print(buffer);
#endif
  writeToSD(buffer, "hdc.csv");

  // bmx values
  for (uint8_t i = 0; i < (sizeof(bmxValues) / sizeof(bmxValues[0])); i++)
  {
    sprintf_P(buffer, PSTR("%s,%.0f,%s,%02s,%02s\r\n\0"), bmxIDS[i], bmxValues[i], timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
    Serial.print(buffer);
#endif
     writeToSD(buffer, "bmx.csv");
  }

  resetVariables();
}

void checkStandby(bool *standby)
{
  // hat sich innerhalb von 10 sekunden mehr als 100 werte auf
  // einer achse bewegt
  if ((sumAccY > standbyThreshold) || (sumAccX > standbyThreshold) || (sumAccZ > standbyThreshold))
  {
#ifdef DEBUG_ENABLED
    Serial.println("Stand by off");
#endif
    // setze zähler zurück
    standbycounter = 0;
    *standby = false;
  }
  else if (!*standby)
  {
    // zähler wird erhöht
    standbycounter += 1;
#ifdef DEBUG_ENABLED
    Serial.print("Stand by detected, increasing counter: ");
    Serial.println(standbycounter);
#endif
  }
  // nach 120 sekunden in denen der standbymodus bestätigt wurde
  // schalte in standby modus
  if (standbycounter > standbyThresholdTime)
  {
    *standby = true;
  }
}

bool submitValues()
{
  // for every file do a request!
  for (int i = 0;  i < (sizeof(fileNames) / sizeof(fileNames[0])) ; i++)
  {
    // close any connection before send a new request.
    // This will free the socket on the WiFi shield
    if (client.connected())
    {
      client.stop();
      delay(1000);
    }
    bool connected = false;
    char _server[strlen_P(server)];
    strcpy_P(_server, server);
    for (uint8_t timeout = 2; timeout != 0; timeout--)
    {
      connected = client.connect(_server, port);
      if (connected == true)
      {
        myFile = SD.open(fileNames[i]);
        if (myFile) {
          clength = myFile.size();
          myFile.close();
        }
        // determine amount of measurements
        char header[750];
        // construct the HTTP POST request:
        sprintf_P(header,
                  PSTR("POST /boxes/%s/data HTTP/1.1\nHost: %s\nContent-Type: text/csv\nConnection: keep-alive\nContent-Length: %i\n\n"),
                  SENSEBOX_ID, server, clength);
#ifdef DEBUG_ENABLED
        Serial.print(header);
#endif
        // send the HTTP POST request:
        client.print(header);
        writeMeasurementsToClient(fileNames[i]);
        // send empty line to end the request
        client.println();
        uint16_t timeout = 0;
        // allow the response to be computed
        while (timeout <= 20000)
        {
          delay(10);
          timeout = timeout + 10;
          if (client.available())
          {
            break;
          }
        }
        while (client.available())
        {
          char c = client.read();
#ifdef DEBUG_ENABLED
          Serial.print(c);
#endif
          // if the server's disconnected, stop the client:
          if (!client.connected())
          {
#ifdef DEBUG_ENABLED
            Serial.println();
            Serial.println(F("disconnecting from server."));
#endif
            client.stop();
            break;
          }
        }
#ifdef DEBUG_ENABLED
        Serial.println(F("done!"));
#endif
        // reset number of measurements
        break;
      }
      delay(1000);
    }
    if (connected == false)
    {
      // Reset durchführen
#ifdef DEBUG_ENABLED
      Serial.println(F("connection failed."));
#endif
    return false;
    }
  }
  // after values have been sent remove everything from sd
  resetSD();
  return true;
}

void writeMeasurementsToClient(char* fileName) {
  char line[100];
  long counter = 0;
  myFile = SD.open(fileName);
  while (myFile.available()) {
    char c = myFile.read();
    if (c == '\n') {
      client.print(line);
#ifdef DEBUG_ENABLED
      Serial.print(line);
#endif
      counter = 0;
      memset(line, 0, sizeof(line));
    }
    line[counter++] = c;
  }
  myFile.close();
}
