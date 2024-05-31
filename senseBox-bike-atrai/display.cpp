#include "display.h"

Adafruit_SSD1306 SBDisplay::display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
QRCode SBDisplay::qrcode;
Adafruit_MAX17048 SBDisplay::maxlipo;

TaskHandle_t xBicycleAnimationTaskHandle;
bool isBicycleAnimationShowing = false;
String loadingMessage;
float loadingProgress;

void SBDisplay::bicycleAnimationTask(void *pvParameter) {
  int dsplW = 128;
  int dsplH = 64;
  int prgsW = 120;
  int prgsH = 2;

  while (1) {
    for (int i = 0; i < 36; i++) {
      display.clearDisplay();
      display.drawBitmap(32, -10, bicycle_loading_bitmap[i], 64, 64, 1);  //this displays each frame hex value
      drawProgressbar(4, (dsplH - 12) - prgsH - 8, prgsW, prgsH, loadingProgress * 100);
      display.setCursor(4, dsplH - 12);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.println(loadingMessage);
      drawBattery(0, 0, 16, 4);
      display.display();
      delay(100);

      if (!isBicycleAnimationShowing) {
        vTaskDelete(NULL);
      }
    }

    vTaskDelay(0);
  }
}

void SBDisplay::begin() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.setRotation(2);
  display.display();
  delay(100);
  display.clearDisplay();

  if (!maxlipo.begin()) {
    Serial.println(F("Couldnt find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    while (1)
      delay(10);
  }
}

void SBDisplay::drawProgressbar(int x, int y, int width, int height, int progress) {
  progress = progress > 100 ? 100 : progress;  // set the progress value to 100
  progress = progress < 0 ? 0 : progress;      // start the counting to 0-100
  float bar = ((float)(width - 1) / 100) * progress;
  display.drawRect(x, y, width, height + 4, WHITE);
  display.fillRect(x + 2, y + 2, bar - 2, height, WHITE);  // initailize the graphics fillRect(int x, int y, int width, int height)
}

void SBDisplay::drawBattery(int x, int y, int width, int height) {
  drawProgressbar(x, y, width, height, maxlipo.cellPercent());
  display.fillRect(x + width, y + 2, 2, height, WHITE);
  if (maxlipo.chargeRate() > 0) {
    display.setCursor(x + width + 4, y + 1);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.println("+");
    display.setCursor(0, 0);
  }
}



void SBDisplay::showLoading(String msg, float val) {
  if (!isBicycleAnimationShowing) {
    isBicycleAnimationShowing = true;
    xTaskCreate(&bicycleAnimationTask, "bicycle_animation_task", 1536, (void *)NULL, 1, &xBicycleAnimationTaskHandle);
  }
  loadingMessage = msg;
  loadingProgress = val;
  // int dsplW = 128;
  // int dsplH = 64;
  // int prgsW = 120;
  // int prgsH = 8;
  // display.clearDisplay();
  // drawProgressbar(4, (dsplH - 12) - prgsH - 4, prgsW, prgsH, val * 100);
  // display.setCursor(4, dsplH - 12);
  // display.setTextSize(1);
  // display.setTextColor(WHITE, BLACK);
  // display.println(msg);
  // display.display();
}

void SBDisplay::showSystemStatus() {
  if (isBicycleAnimationShowing) {
    isBicycleAnimationShowing = false;
  }

  display.clearDisplay();


  // display.clearDisplay();
  display.setCursor(0, 0);
  display.println("");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);

  display.println(F("Batt Percent: "));
  display.setTextSize(1);
  display.print(maxlipo.cellPercent(), 1);
  display.println(" %");

  display.println("");

  display.setTextSize(1);
  display.println(F("(Dis)Charge rate : "));
  display.setTextSize(1);
  display.print(maxlipo.chargeRate(), 1);
  display.println(" %/hr");

  drawBattery(0, 0, 16, 4);

  display.display();
}

void SBDisplay::drawQrCode(const char *qrStr, const char *lines[]) {
  if (isBicycleAnimationShowing) {
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

  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
      int newX = offset_x + (x * 2);
      int newY = offset_y + (y * 2);

      if (qrcode_getModule(&qrcode, x, y)) {
        display.fillRect(newX, newY, 2, 2, 1);
      } else {
        display.fillRect(newX, newY, 2, 2, 0);
      }
    }
  }
  display.setTextColor(1, 0);
  for (int i = 0; i < 4; i++) {
    display.setCursor(cursor_start_x, cursor_start_y + font_height * i);
    display.println(lines[i]);
  }
  drawBattery(0, 0, 16, 4);
  display.display();
}
