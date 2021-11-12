#include "variables/variables.h"

void initGPS()
{
  Wire.begin();

  if (myGNSS.begin() == false) //Connect to the Ublox module using Wire port
  {
#ifdef DEBUG_ENABLED
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
#endif
    while (1)
      ;
  }
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.setNavigationFrequency(2);  //Produce two solutions per second
  myGNSS.setAutoPVT(true);           //Tell the GNSS to "send" each solution
  myGNSS.saveConfiguration();        //Optional: Save the current settings to flash and BBR
};

bool isGPSvalid()
{

  // pdop ???
  // myGNSS.getPDOP() 
  // discard measurements with pdop greater than 3 maybe
  if (myGNSS.getInvalidLlh() != true && myGNSS.getLatitude() != 0 && myGNSS.getTimeValid() && myGNSS.getDateValid() && myGNSS.getFixType() == 3)
    return true;
  else
    return false;
}

void setTimestamp(){
  char timestamp[124];
  sprintf(timestamp, "%02d-%02d-%02dT%02d:%02d:%02dZ\0",
        myGNSS.getYear(),
        myGNSS.getMonth(),
        myGNSS.getDay(),
        myGNSS.getHour(),
        myGNSS.getMinute(),
        myGNSS.getSecond());

  strcpy_P(timestampGlobal, timestamp);

}

void setGPS(){
  float latitude = myGNSS.getLatitude() / (float)10000000;
  float longitude = myGNSS.getLongitude() / (float)10000000;
  char lng[20];
  char lat[20];
  double homeLon[] = {minLon, minLon, maxLon, maxLon};
  double homeLat[] = {minLat, maxLat, maxLat, minLat};

  // set GPS to fixed position, when the actual position is inside the fence.
  if (defLat > 1 && defLon > 1) { // check if default coordiante is set.
    if (pnpoly(4, homeLat, homeLon, latitude, longitude)) {
      dtostrf(defLon, 2, 7, lng);
      dtostrf(defLat, 1, 7, lat);
    } else {
      dtostrf(longitude, 2, 7, lng);
      dtostrf(latitude, 1, 7, lat);
    }
  } else {
      dtostrf(longitude, 2, 7, lng);
      dtostrf(latitude, 1, 7, lat);
  }

    
  strcpy_P(lngGlobal, lng);
  strcpy_P(latGlobal, lat);
  
}
