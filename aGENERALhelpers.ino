void resetVariables()
{
  sumAccZ = 0;
  sumAccY = 0;
  sumAccX = 0;
}
void connectToWifi()
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
#ifdef DEBUG_ENABLED
  Serial.print(F("Attempting to connect to SSID: "));
  Serial.println(ssid);
#endif
  // Connect to WPA/WPA2 network. Change this line if using open or WEP
  // network
  status = WiFi.begin(ssid, pass);
  // wait 10 seconds for connection:
#ifdef DEBUG_ENABLED
  Serial.print(F("Waiting 10 seconds for connection..."));
#endif
  delay(10000);
#ifdef DEBUG_ENABLED
  Serial.println(F("done."));
#endif
}

void getMeasurements()
{
  float pm10, pm25;
  SDS.read(&pm10, &pm25);
  float bmxValues[] = {sumAccX, sumAccY, sumAccZ};

  int distanceR = (int)left.ping_cm();
  if (distanceR == 0) distanceR = 400;


  int distanceL = (int)left.ping_cm();
  if (distanceL == 0) distanceL = 400;
 
  long speed = myGNSS.getGroundSpeed();
  float speedKmH = speed * 0.0036;
  int distanceValues[2] = {distanceL, distanceR};

  float sensorValues[5] = {HDC.readTemperature(), HDC.readHumidity(), pm10, pm25, speedKmH};

  // sensor values
  char buffer[750];
  for (uint8_t i = 0; i < (sizeof(sensorValues) / sizeof(sensorValues[0])); i++)
  {
    sprintf_P(buffer, PSTR("%s,%f,%s,%02s,%02s\r\n\0"), sensorIDS[i], sensorValues[i], timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
    Serial.print(buffer);
#endif
    writeToSD(buffer, "sensor.csv");
  }

  // bmx values
  for (uint8_t i = 0; i < (sizeof(bmxValues) / sizeof(bmxValues[0])); i++)
  {
    sprintf_P(buffer, PSTR("%s,%f,%s,%02s,%02s\r\n\0"), bmxIDS[i], bmxValues[i], timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
    Serial.print(buffer);
#endif
    writeToSD(buffer, "bmx.csv");
  }
  memset(buffer, 0, sizeof(buffer));
  // distance values
  for (uint8_t i = 0; i < (sizeof(distanceValues) / sizeof(distanceValues[0])); i++)
  {
    sprintf_P(buffer, PSTR("%s,%d,%s,%02s,%02s\r\n\0"), distanceIDS[i], distanceValues[i], timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
    Serial.print(buffer);
#endif
    writeToSD(buffer, "distanz.csv");
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
  if (WiFi.status() != WL_CONNECTED)
  {
#ifdef DEBUG_ENABLED
    Serial.println("No wifi; connecting again");
#endif
    WiFi.disconnect();
    delay(1000); // wait 1s
    WiFi.begin(ssid, pass);
    delay(5000); // wait 5s
  }
  // for every file do a request!
  for (int i = 0; i < 3; i++)
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
