#include "Display.h"
#include <SPI.h>
// #include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <QRCodeGenerator.h>
#include <Adafruit_MAX1704X.h>
#include "bicycle_loading_bitmap.h"

// Adafruit_SSD1306 SBDisplay::display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
QRCode SBDisplay::qrcode;

float batteryCharge = 0;

TaskHandle_t xBicycleAnimationTaskHandle;
bool isBicycleAnimationShowing = false;
String loadingMessage;
float loadingProgress;

String bleId = "";

void SBDisplay::bicycleAnimationTask(void *pvParameter)
{
  int dsplW = 128;
  int dsplH = 64;
  int prgsW = 120;
  int prgsH = 2;

  while (1)
  {
    for (int i = 0; i < 36; i++)
    {
      display.clearDisplay();
      display.drawBitmap(32, -10, bicycle_loading_bitmap[i], 64, 64, 1); // this displays each frame hex value
      drawProgressbar(4, (dsplH - 12) - prgsH - 8, prgsW, prgsH, loadingProgress * 100);
      display.setCursor(4, dsplH - 12);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.println(loadingMessage);
      // drawBattery(0, 0, 16, 4);
      display.display();
      vTaskDelay(pdMS_TO_TICKS(100));

      if (!isBicycleAnimationShowing)
      {
        vTaskDelete(xBicycleAnimationTaskHandle);
      }
    }
  }
}

void SBDisplay::begin()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.setRotation(2);
  display.display();
  delay(100);
  display.clearDisplay();
}

void SBDisplay::drawProgressbar(int x, int y, int width, int height, int progress)
{
  progress = progress > 100 ? 100 : progress; // set the progress value to 100
  progress = progress < 0 ? 0 : progress;     // start the counting to 0-100
  float bar = ((float)(width - 1) / 100) * progress;
  display.drawRect(x, y, width, height + 4, WHITE);
  display.fillRect(x + 2, y + 2, bar - 3, height, WHITE); // initailize the graphics fillRect(int x, int y, int width, int height)
}

void SBDisplay::drawBattery(int x, int y, int width, int height)
{
  batteryCharge = BatterySensor::getBatteryCharge();
  drawProgressbar(x, y, width, height, batteryCharge);
  display.fillRect(x + width, y + 2, 2, height, WHITE);
  if (BatterySensor::getBatteryChargeRate() > 0)
  {
    display.setCursor(x + width + 4, y + 1);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.println("+");
    display.setCursor(0, 0);
  }
}

void SBDisplay::showLoading(String msg, float val)
{
  if (!isBicycleAnimationShowing)
  {
    isBicycleAnimationShowing = true;
    xTaskCreate(&bicycleAnimationTask, "bicycle_animation_task", 1536, (void *)NULL, 1, &xBicycleAnimationTaskHandle);
  }
  loadingMessage = msg;
  loadingProgress = val;
}

void SBDisplay::showSystemStatus()
{
  if (isBicycleAnimationShowing)
  {
    isBicycleAnimationShowing = false;
  }

  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);

  display.println(F("Batt Percent: "));
  display.setTextSize(1);
  display.print(batteryCharge, 1);
  display.println(" %");

  drawBattery(0, 0, 16, 4);

  display.display();
}

void SBDisplay::showConnectionScreen()
{
  if (isBicycleAnimationShowing)
  {
    isBicycleAnimationShowing = false;
  }

  // String bleIdBrackets = "[" + bleId + "]";
  // String name = "senseBox:bike " + bleIdBrackets;
  // String bleIdBegin = bleIdBrackets.substring(0, bleIdBrackets.length() / 2);
  // String bleIdEnd = bleIdBrackets.substring(bleIdBrackets.length() / 2);
  // const char *message[] = {
  //     "senseBox",
  //     "bike",
  //     bleIdBegin.c_str(),
  //     bleIdEnd.c_str()};

  // drawQrCode(name.c_str(), message);

  drawBattery(0, 0, 16, 4);
  display.display();
}

void SBDisplay::drawQrCode(const char *qrStr, const char *lines[])
{
  if (isBicycleAnimationShowing)
  {
    isBicycleAnimationShowing = false;
  }

  display.clearDisplay();
  display.setTextSize(1);

  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, ECC_MEDIUM, qrStr);

  // Text starting point
  int cursor_start_y = 10;
  int cursor_start_x = 4;
  int font_height = 12;

  // QR Code Starting Point
  int offset_x = 62;
  int offset_y = 3;

  for (int y = 0; y < qrcode.size; y++)
  {
    for (int x = 0; x < qrcode.size; x++)
    {
      int newX = offset_x + (x * 2);
      int newY = offset_y + (y * 2);

      if (qrcode_getModule(&qrcode, x, y))
      {
        display.fillRect(newX, newY, 2, 2, 1);
      }
      else
      {
        display.fillRect(newX, newY, 2, 2, 0);
      }
    }
  }
  display.setTextColor(1, 0);
  for (int i = 0; i < 4; i++)
  {
    display.setCursor(cursor_start_x, cursor_start_y + font_height * i);
    display.println(lines[i]);
  }
}

void SBDisplay::readBleId()
{
  // bleId = SenseBoxBLE::getMCUId();
}