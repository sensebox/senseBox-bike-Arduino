void initGPS()
{
    Serial1.begin(9600);

};
void setTimestamp(){
  char timestamp[124];

  sprintf(timestamp, "20%d-%02d-%02dT%02d:%02d:%02dZ\0",
        fix.dateTime.year,
        fix.dateTime.month,
        fix.dateTime.date,
        fix.dateTime.hours,
        fix.dateTime.minutes,
        fix.dateTime.seconds);

  strcpy_P(timestampGlobal, timestamp);

}

void setGPS(){
  double latitude = fix.latitude();
  double longitude = fix.longitude();
  char lng[20];
  char lat[20];
  dtostrf(longitude, 2, 7, lng);
  dtostrf(latitude, 1, 7, lat);
    
    
  strcpy_P(lngGlobal, lng);
  strcpy_P(latGlobal, lat);
  
}

void setGPS(){
  double latitude = fix.latitude();
  double longitude = fix.longitude();
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