# senseBox:bike 
Mobile Messstation mit der Temperatur, rel. Luftfeuchte, Feinstaub (PM10, PM25), Abstand nach links und rechts sowie die Bodenerschütterung gemessen werden. Die Daten fließen in ein Exponat im [Futurium](https://futurium.de). 


[![senseBox Bike](https://raw.githubusercontent.com/sensebox/senseBox-bike-Arduino/main/senseBox_bike.png "senseBox Bike")](https://raw.githubusercontent.com/sensebox/senseBox-bike-Arduino/main/senseBox_bike.png "senseBox Bike")


# Anleitung 
## Vorbereitungen für Arduino 
 - Installiere das [Board-Support-Package der senseBox](https://docs.sensebox.de/arduino/board-support-package-installieren/ "Board-Support-Package der senseBxo") 
 - Installiere folgende Libraries: 
  - [NewPing](https://www.arduino.cc/reference/en/libraries/newping "NewPing")
  - [SparkFun u-Blox GNSS](https://www.arduino.cc/reference/en/libraries/sparkfun-u-blox-gnss-arduino-library/ "SparkFun u-Blox GNSS")
  - [BMX](https://github.com/sensebox/BMX)
  - [SD](https://www.arduino.cc/en/Reference/SD "SD") For Linux systems
  - [SDConfig](https://github.com/Fuzzer11/SDconfig)
  
## Credentials hinzufügen 
Mit Hilfe der [openSenseMapAPI](https://api.opensensemap.org/) kannst du die Sensor IDs deiner Box abfragen. Der API call lautet `https://api.opensensemap.org/boxes/[:senseBoxID]`.
 - In der Datei `variables/network.h` WiFi Informationen (SSID und Passwort) eingeben 
 - In der Datei `variables/ids.h` senseBox ID und Sensor ID's von der openSenseMap eintragen 

Alternativ kann auf der SDCard eine Datei `BIKE.CFG` (Siehe `BIKE.CFG.example`) angelegt werden, die die Variablen Datenschutzfreundlich initialisiert.

## Upload
- Mit Hilfe der Arduino IDE auf die senseBox hochladen

## Entwickeln
 - Damit du das Projekt in der Arduino IDE öffnen kannst, muss der Name des Ordners mit dem der Einstiegsdatei übereinstimmen.
 - Dafür clone das Projekt wie folgt:

   ``` bash
   $ git clone git@github.com:sensebox/senseBox-bike-Arduino.git futuriumSDNoAp
   ```
  - Um das gesamte Projekt zu öffnen, doppelklicke die `futuriumSDNoAp.ino` Datei. Daraufhin sollte sich die Arduino IDE automatisch öffnen und diese und alle anderen Dateien anzeigen (siehe Screenshot).

    ![Arduino IDE Screenshot](arduino-ide-screenshot.png "Arduino IDE Screenshot")

## Debug 

- In `futuriumSDNoAP.ino` das Makro `#define DEBUG_ENABLED` setzen, damit DEBUG Nachrichten in der seriellen Konsole angezeigt werden.
- ⚠️ **ACHTUNG** Im DEBUG Modus startet die Box erst, wenn der *Serielle Monitor* geöffnet wird. Vor dem Betrieb den DEBUG Modus IMMER ausschalten!
