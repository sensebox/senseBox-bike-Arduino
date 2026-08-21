#include "LED.h"

// Constructor
LED::LED(uint8_t pin, uint16_t numPixels, uint8_t brightness)
    : pixels(numPixels, pin, NEO_GRB + NEO_KHZ800),
      hue(0),
      taskHandle(NULL),
      currentStatus(LEDStatus::STARTING),
      pulseValue(20),
      pulseDirection(1),
      errorLedOn(false),
      lastErrorToggle(0)
{
    pixels.setBrightness(brightness);
}

// Initialize the NeoPixel strip
void LED::begin()
{
    pixels.begin();
}

void LED::ensureTaskRunning()
{
    if (taskHandle == NULL)
    {
        BaseType_t result = xTaskCreate(
            LEDTask,
            "LED Task",
            4096,
            this,
            1,
            &taskHandle);

        if (result != pdPASS)
        {
            taskHandle = NULL;
            Serial.println("LED task FAILED");
        }
    }
}

// Start the FreeRTOS task to update the LED color
void LED::startRainbow()
{
    BaseType_t result = xTaskCreate(
        LEDTask,
        "LED Task",
        4096,
        this,
        1,
        &taskHandle
    );
}

// Stop the FreeRTOS task
void LED::stopRainbow()
{
    if (taskHandle != NULL)
    {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
    }
    // turn off the LED
    pixels.clear();
    pixels.show();
}

void LED::setStatus(LEDStatus status)
{
    currentStatus = status;
    ensureTaskRunning();
}

// Task function to update the LED color
void LED::LEDTask(void *pvParameters)
{
    LED *ledInstance = static_cast<LED *>(pvParameters);

    // Infinite loop to continuously update the LED color
    while (true)
    {
        ledInstance->update();
        vTaskDelay(pdMS_TO_TICKS(20)); // Delay of 20 milliseconds
    }
}

// Update the LED color to the next hue
void LED::update()
{
    switch (currentStatus)
    {
    case LEDStatus::STARTING:
        // Gelb: Initialisierung läuft
        pixels.setPixelColor(0, pixels.Color(255, 180, 0));
        break;

    case LEDStatus::WAITING_BLE:
        // Blau langsam pulsierend
        pixels.setPixelColor(0, pixels.Color(0, 0, pulseValue));

        pulseValue += pulseDirection * 2;

        if (pulseValue >= 250)
        {
            pulseValue = 250;
            pulseDirection = -1;
        }
        else if (pulseValue <= 20)
        {
            pulseValue = 20;
            pulseDirection = 1;
        }
        break;

    case LEDStatus::CONNECTED:
        // Grün: BLE verbunden
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        break;

    case LEDStatus::ERROR:
        // Rot blinkend
        if (millis() - lastErrorToggle >= 500)
        {
            lastErrorToggle = millis();
            errorLedOn = !errorLedOn;
        }

        if (errorLedOn)
        {
            pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        }
        else
        {
            pixels.setPixelColor(0, 0);
        }
        break;
    }
    pixels.show();
}

// Private function to generate rainbow colors across 0-255 positions
uint32_t LED::Wheel(byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}
