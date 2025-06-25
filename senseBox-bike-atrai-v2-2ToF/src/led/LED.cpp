#include "LED.h"

// Constructor
LED::LED(uint8_t pin, uint16_t numPixels, uint8_t brightness)
    : pixels(numPixels, pin, NEO_GRB + NEO_KHZ800), hue(0), taskHandle(NULL)
{
    pixels.setBrightness(brightness);
}

// Initialize the NeoPixel strip
void LED::begin()
{
    pixels.begin();
}

// Start the FreeRTOS task to update the LED color
void LED::startRainbow()
{
    // Create the LED update task
    xTaskCreate(
        LEDTask,    // Task function
        "LED Task", // Name of the task (for debugging)
        1024,       // Stack size (in words)
        this,       // Task input parameter (this LED instance)
        1,          // Priority of the task
        &taskHandle // Task handle
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

// Task function to update the LED color
void LED::LEDTask(void *pvParameters)
{
    LED *ledInstance = static_cast<LED *>(pvParameters);

    // Infinite loop to continuously update the LED color
    while (true)
    {
        ledInstance->update();
        vTaskDelay(pdMS_TO_TICKS(5)); // Delay of 10 milliseconds
    }
}

// Update the LED color to the next hue
void LED::update()
{
    pixels.setPixelColor(0, Wheel(hue));
    pixels.show();

    hue += 1;
    if (hue > 255)
    {
        hue = 0;
    }
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
