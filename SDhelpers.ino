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

  for (int i = 0; i < 3; i++) {
    if (SD.exists(fileNames[i])) {
      empty = false;
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
