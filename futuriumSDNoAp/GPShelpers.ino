void initGPS()
{
    Serial1.begin(9600);

};
void setTimestamp(){
  char timestamp[124];
  sprintf(timestamp, "%d-%02d-%02dT%02d:%02d:%02dZ\0",
        fix.dateTime.year,
        fix.dateTime.month,
        fix.dateTime.day,
        fix.dateTime.hours,
        fix.dateTime.minutes,
        fix.dateTime.seconds);

  strcpy_P(timestampGlobal, timestamp);

}

void setGPS(){
  float latitude = fix.latitude();
  double longitude = fix.longitude();
  char lng[20];
  char lat[20];
  dtostrf(longitude, 2, 7, lng);
  dtostrf(latitude, 1, 7, lat);
    
    
  strcpy_P(lngGlobal, lng);
  strcpy_P(latGlobal, lat);
  
}
