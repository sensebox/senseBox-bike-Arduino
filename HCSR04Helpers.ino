void initUltrasonic()
{
  //pinmodes for ultrasonic
  pinMode(TRIGGER_LEFT, OUTPUT);
  pinMode(TRIGGER_RIGHT, INPUT);
  pinMode(TRIGGER_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);
};

void handleLeft()
{
  int distance = (int)left.ping_cm();
  if (distance == 0) distance = 400;

  char bufferDistance[100];
  if (distance < 150)
  {
    if (time_start > timeoutLeft + 2500)
    {
      timeoutLeft = millis();
      sprintf_P(bufferDistance, PSTR("%s,%d,%s,%02s,%02s\r\n\0"), distLeftID, distance, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
      Serial.print(bufferDistance);
#endif
    writeToSD(bufferDistance, "distanz.csv");

    }
  }
}

void handleRight()
{
  int distance = (int)right.ping_cm();
  if (distance == 0) distance = 400;

  char bufferDistance[100];
  if (distance < 150)
  {
    if (time_start > timeoutRight + 2500)
    {
      timeoutRight = millis();
      
      sprintf_P(bufferDistance, PSTR("%s,%d,%s,%02s,%02s\r\n\0"), distRightID, distance, timestampGlobal, lngGlobal, latGlobal);
#ifdef DEBUG_ENABLED
      Serial.print(bufferDistance);
#endif
    writeToSD(bufferDistance, "distanz.csv");
    }
  }
}
