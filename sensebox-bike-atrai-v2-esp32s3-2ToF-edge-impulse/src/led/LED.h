#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

class LED
{
public:
    LED(uint16_t numPixels, uint8_t pin, uint8_t brightness = 50);
    void begin();
    void showGreen();
    void showYellow();
    void showRed();
    void startRainbow();
    void stopRainbow();

private:
    uint32_t Wheel(byte WheelPos);
    void update();
    static void LEDTask(void *pvParameters);

    Adafruit_NeoPixel pixels;
    uint16_t hue;
    TaskHandle_t taskHandle;
};

#endif
