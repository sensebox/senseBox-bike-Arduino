#ifndef LED_H
#define LED_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

enum class LEDStatus
{
    STARTING,
    WAITING_BLE,
    CONNECTED,
    ERROR
};

class LED
{
public:
    LED(uint8_t pin, uint16_t numPixels, uint8_t brightness = 50);

    void begin();
    void startRainbow();
    void stopRainbow();
    void setStatus(LEDStatus status);

private:
    uint32_t Wheel(byte WheelPos);
    void update();
    static void LEDTask(void *pvParameters);
    void ensureTaskRunning();
    
    Adafruit_NeoPixel pixels;
    uint16_t hue;
    TaskHandle_t taskHandle;

    LEDStatus currentStatus;
    uint8_t pulseValue;
    int8_t pulseDirection;
    bool errorLedOn;
    unsigned long lastErrorToggle;
};

#endif
