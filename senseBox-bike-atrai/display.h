#ifndef MY_DISPLAY_H
#define MY_DISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <QRCodeGenerator.h>
#include <Adafruit_MAX1704X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class SBDisplay {
public:
  static void begin();
  static void showSystemStatus();
  static void drawQrCode(const char *qrStr, const char *lines[]);
  static void showLoading(String msg, float val);
  static void drawProgressbar(int x, int y, int width, int height, int progress);

private:
  static Adafruit_SSD1306 display;
  static QRCode qrcode;
  static Adafruit_MAX17048 maxlipo;
};
#endif  // MY_DISPLAY_H