#ifndef MY_DISPLAY_H
#define MY_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <QRCodeGenerator.h>

#include "../ble/BLEModule.h"
#include "../sensors/BatterySensor/BatterySensor.h"

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

class SBDisplay
{
public:
  static void begin();
  static void showSystemStatus();
  static void showConnectionScreen();
  static void drawQrCode(const char *qrStr, const char *lines[]);
  static void showLoading(String msg, float val);
  static void drawProgressbar(int x, int y, int width, int height, int progress);
  static void drawBattery(int x, int y, int width, int height);
  static void bicycleAnimationTask(void *pvParams);
  static void readBleId();
  static void showError(String msg);

private:
  static Adafruit_SSD1306 display;
  static QRCode qrcode;
};
#endif // MY_DISPLAY_H