# senseBox:bike 
Mobile Messstation mit der Temperatur, rel. Luftfeuchte, Feinstaub(PM10, PM25), Abstand nach links und rechts sowie die Bodenerschütterung gemessen werden. Die Daten fließen in ein Exponat im Futurium. 


[![senseBox Bike](https://raw.githubusercontent.com/sensebox/senseBox-bike-Arduino/main/senseBox_bike.png "senseBox Bike")](https://raw.githubusercontent.com/sensebox/senseBox-bike-Arduino/main/senseBox_bike.png "senseBox Bike")


# Anleitung 
## Vorbereitungen für Arduino 
 - Installiere das [Board-Support-Package der senseBox](https://docs.sensebox.de/arduino/board-support-package-installieren/ "Board-Support-Package der senseBxo") 
 - Installiere folgende Libraries: 
  - [NewPing](http://https://www.arduino.cc/reference/en/libraries/newping "NewPing")
  - [SparkFun u-Blox GNSS](https://www.arduino.cc/reference/en/libraries/sparkfun-u-blox-gnss-arduino-library/tp:// "SparkFun u-Blox GNSS")
  - [BMX](https://github.com/sensebox/BMX)
  
## Credentials hinzufügen 
 - In der Datei `variables/network.h` WiFi Informationen (SSID und Passwort) eingeben 
 - In der Datei `variables/ids.h` senseBox ID und Sensor ID's von der openSenseMap eintragen 

## Upload
Mithilfer der Arduino IDE auf die senseBox hochladen 

## Debug 

In `futuriumSDNoAP.ino` das Makro `#define DEBUG_ENABLED` setzen, damit DEBUG Nachrichten in der Seriellen Konsole angezeigt werden. <b>!!!ACHTUNG</b> Im Debug Modus startet die Box erst, wenn der Serielle Monitor geöffnet wird. Vor dem Betrieb den Debug Modus IMMER ausschalten !

