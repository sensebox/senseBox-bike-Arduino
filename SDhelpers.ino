#include "variables/variables.h"

void writeToSD(char *data, char *fileName)
{
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile)
  {
    myFile.print(data);
    myFile.close();
  }
  else
  {
#ifdef DEBUG_ENABLED
    Serial.print("Opening file ");
    Serial.print(fileName);
    Serial.println(" failed");
#endif
  }
}

void initSD()
{
  if (!SD.begin(28))
  {
#ifdef DEBUG_ENABLED
    Serial.println("SD Init failed");
#endif
  }
}
void resetSD()
{
  for (int i = 0; i < 3; i++)
  {
    SD.remove(fileNames[i]);
  }
}

bool sdisempty() {
  bool empty = true;
Serial.print("SDisempty ");
  for (int i = 0; i < 3; i++) {
    if (SD.exists(fileNames[i])) {
      empty = false;
#ifdef DEBUG_ENABLED
     Serial.println(" false");
#endif
    }
  }
  return empty;
}
void checkForFiles()
{
  bool existing = false;
  for (int i = 0; i < 3; i++)
  {
    if (SD.exists(fileNames[i]))
      existing = true;
  }
  if (existing)
  {
#ifdef DEBUG_ENABLED
    Serial.println("File existed; Uploading now instead of measuring");
#endif
    connectToWifi();
    if (WiFi.status() != WL_CONNECTED)
    {
#ifdef DEBUG_ENABLED
      Serial.println("No wifi found going back to measuring");
#endif
    }
    else
    {
#ifdef DEBUG_ENABLED
      Serial.println("Wifi found; uploading values");
#endif
      submitValues();
      while (true)
        ;
    }
  }
  else
  {
    for (int i = 0; i < 3; i++)
    {
      myFile = SD.open(fileNames[i], FILE_WRITE);
      myFile.close();
    }
  }
}

boolean readConfiguration( char* configFile )
{
  /*
   * Length of the longest line expected in the config file.
   * The larger this number, the more memory is used
   * to read the file.
   * You probably won't need to change this number.
   */
  int maxLineLength = 127;
  SDConfig cfg;
  
  // Open the configuration file.
  if (!cfg.begin(configFile, maxLineLength)) {
#ifdef DEBUG_ENABLED
    Serial.print(" Failed to open configuration file: ");
    Serial.println(configFile);
#endif
    return false;
  }

  // Read each setting from the file.
  while (cfg.readNextSetting()) {
    // Put a nameIs() block here for each setting you have.
    // ssid
    if (cfg.nameIs("SSID")) {
      ssid = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read SSID: "); Serial.println(ssid);
#endif      
    } else if (cfg.nameIs("PSK")) {
      pass = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read PSK: "); Serial.println(pass);
#endif      
    } else if (cfg.nameIs("SENSEBOX_ID")) {
      SENSEBOX_ID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read SENSEBOX_ID: "); Serial.println(SENSEBOX_ID);
#endif      
    } else if (cfg.nameIs("TEMP_ID")) {
      tempID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read TEMP_ID: "); Serial.println(tempID);
#endif      
    } else if (cfg.nameIs("HUMI_ID")) {
      humiID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read HUMI_ID: "); Serial.println(humiID);
#endif      
    } else if (cfg.nameIs("DIST_L_ID")) {
      distLeftID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read DIST_L_ID: "); Serial.println(distLeftID);
#endif      
    } else if (cfg.nameIs("DIST_R_ID")) {
      distRightID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read DIST_R_ID: "); Serial.println(distRightID);
#endif      
    } else if (cfg.nameIs("PM10_ID")) {
      pm10ID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read PM10_ID: "); Serial.println(pm10ID);
#endif      
    } else if (cfg.nameIs("PM25_ID")) {
      pm25ID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read PM25_ID: "); Serial.println(pm25ID);
#endif      
    } else if (cfg.nameIs("ACC_X_ID")) {
      accXID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read ACC_X_ID: "); Serial.println(accXID);
#endif      
    } else if (cfg.nameIs("ACC_Y_ID")) {
      accYID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read ACC_Y_ID: "); Serial.println(accYID);
#endif      
    } else if (cfg.nameIs("ACC_Z_ID")) {
      accZID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read ACC_Z_ID: "); Serial.println(accZID);
#endif      
    } else if (cfg.nameIs("SPEED_ID")) {
      speedID = cfg.copyValue();
#ifdef DEBUG_ENABLED
      Serial.print("Read SPEED_ID: "); Serial.println(speedID);
#endif      
    } else if (cfg.nameIs("HOME_MINLON")) {
      minLon = strtod(cfg.copyValue(), NULL);
#ifdef DEBUG_ENABLED
      Serial.print("Read HOME_MINLON: "); Serial.println(minLon, 7);
#endif      
    } else if (cfg.nameIs("HOME_MAXLON")) {
      maxLon = strtod(cfg.copyValue(), NULL);
#ifdef DEBUG_ENABLED
      Serial.print("Read HOME_MAXLON: "); Serial.println(maxLon, 7);
#endif      
    } else if (cfg.nameIs("HOME_MINLAT")) {
      minLat = strtod(cfg.copyValue(), NULL);
#ifdef DEBUG_ENABLED
      Serial.print("Read HOME_MINLAT: "); Serial.println(minLat, 7);
#endif      
    } else if (cfg.nameIs("HOME_MAXLAT")) {
      maxLat = strtod(cfg.copyValue(), NULL);
#ifdef DEBUG_ENABLED
      Serial.print("Read HOME_MAXLAT: "); Serial.println(maxLat, 7);
#endif      
    } else if (cfg.nameIs("HOME_DEFLAT")) {
      defLat = strtod(cfg.copyValue(), NULL);
#ifdef DEBUG_ENABLED
      Serial.print("Read HOME_DEFLAT: "); Serial.println(defLat, 7);
#endif      
    } else if (cfg.nameIs("HOME_DEFLON")) {
      defLon = strtod(cfg.copyValue(), NULL);
#ifdef DEBUG_ENABLED
      Serial.print("Read HOME_DEFLON: "); Serial.println(defLon, 7);
#endif      

    } else {
      // report unrecognized names.
#ifdef DEBUG_ENABLED
      Serial.print("Unknown name in config: ");
      Serial.println(cfg.getName());
#endif
    }
  }
  // clean up
  cfg.end();
  return true;
}

void dumpConfiguration()
{
  Serial.println("Current Config");
  Serial.print("SSID:        "); Serial.println(ssid);
  Serial.print("PSK:         "); Serial.println(pass );
  Serial.print("SENSEBOX_ID: "); Serial.println(SENSEBOX_ID);
  Serial.print("TEMP_ID:     "); Serial.println(tempID);
  Serial.print("HUMI_ID:     "); Serial.println(humiID);
  Serial.print("DIST_L_ID:   "); Serial.println(distLeftID);
  Serial.print("DIST_R_ID:   "); Serial.println(distRightID);
  Serial.print("PM10_ID:     "); Serial.println(pm10ID);
  Serial.print("PM25_ID:     "); Serial.println(pm25ID);
  Serial.print("ACC_X_ID:    "); Serial.println(accXID);
  Serial.print("ACC_Y_ID:    "); Serial.println(accYID);
  Serial.print("ACC_Z_ID:    "); Serial.println(accZID);
  Serial.print("SPEED_ID:    "); Serial.println(speedID);
  Serial.print("HOME_MINLON: "); Serial.println(minLon, 7);
  Serial.print("HOME_MAXLON: "); Serial.println(maxLon, 7);
  Serial.print("HOME_MAXLAT: "); Serial.println(minLat, 7);
  Serial.print("HOME_MINLAT: "); Serial.println(maxLat, 7);
  Serial.print("HOME_DEFLON: "); Serial.println(defLon, 7);
  Serial.print("HOME_DEFLAT: "); Serial.println(defLat, 7);
}
