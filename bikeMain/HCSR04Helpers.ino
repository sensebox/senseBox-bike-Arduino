#include "variables/variables.h"

void initUltrasonic()
{
  //pinmodes for ultrasonic
  pinMode(TRIGGER_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  // pinMode(TRIGGER_RIGHT, OUTPUT);
  // pinMode(ECHO_RIGHT, INPUT);
};

long getCm(){
  long duration, distance;
  
  // Sende Triggerpuls
  digitalWrite(TRIGGER_LEFT, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_LEFT, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_LEFT, LOW);

  // Messung der Echo-Dauer
  duration = pulseIn(ECHO_LEFT, HIGH);

  // Berechne Distanz aus Echo-Dauer
  distance = (duration / 2) / 29.1;
  if(distance > 400) { distance = 400;}

  return distance; 
}



void handleDistance(){
  long currentTime = millis();
  if(distance < 150 ) {
    if ( !recording ) { 
      if( currentTime - last > 1000) {
        #ifdef DEBUG_ENABLED
        Serial.print("Ueberholmanöver entdeckt: ");
        Serial.println(distance);
        #endif
        last = millis();
        recording = true;
        char bufferDistance [750];
        sprintf_P(bufferDistance, PSTR("%s,%d,%s,%02s,%02s\r\n\0"), distLeftID, distance, timestampGlobal, lngGlobal, latGlobal);
        #ifdef DEBUG_ENABLED
              Serial.print(bufferDistance);
        #endif
    writeToSD(bufferDistance, "distanz.csv");
      }
    }
  }
  if(distance > 150 && recording) 
  {
    if ( recording )
    {
      #ifdef DEBUG_ENABLED
      Serial.print("Ueberholmanoever vorbei: ");
      Serial.println(distance);
      #endif    
    }
      recording = false;
  }

}
