void printMeasurements() {
    // Ausgabe der Messwerte im seriellen Monitor
  Serial.println("Temperature: " + String(temp) + " °C");
  Serial.println("Humidity: " + String(humi) + " %");
  Serial.println("PM1: " + String(pm1) + " µg/m³");
  Serial.println("PM2.5: " + String(pm25) + " µg/m³");
  Serial.println("PM4: " + String(pm4) + " µg/m³");
  Serial.println("PM10: " + String(pm10) + " µg/m³");
  Serial.println("Acceleration (X, Y, Z): " + String(sumAccX) + ", " + String(sumAccY) + ", " + String(sumAccZ) + " g");
  Serial.println("Distance: " + String(distance) + " cm");
}