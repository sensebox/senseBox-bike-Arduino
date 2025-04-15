#include "../globals.h"
#include "LED.h"

// Constructor
LED::LED(uint16_t numPixels, uint8_t pin, uint8_t brightness)
    : pixels(1, 14, NEO_GRB + NEO_KHZ800), hue(0), taskHandle(NULL)
{
    // pixels.setBrightness(brightness);
}

// Initialize the NeoPixel strip
void LED::begin()
{
    pixels.begin();
    pixels.setBrightness(30);  
}

// Show a green on the LED
void LED::showGreen()
{
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
}
// Show a yellow on the LED
void LED::showYellow()
{
    pixels.setPixelColor(0, pixels.Color(150, 150, 0));
    pixels.show();
}
// Show a red on the LED
void LED::showRed()
{
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
}

// Start the FreeRTOS task to update the LED color
void LED::startRainbow()
{
        
    // Create the LED update task
    xTaskCreatePinnedToCore(
        LEDTask,    // Task function
        "LED Task", // Name of the task (for debugging)
        1024,       // Stack size (in words)
        this,       // Task input parameter (this LED instance)
        1,          // Priority of the task
        &this->taskHandle, // Task handle
        0           // Core to run the task on (0 or 1)
    );
    Serial.println("task done");
    delay(5);
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
        Serial.println("hi");
        // if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE)
        // {
            ledInstance->update();
            // xSemaphoreGive(i2c_mutex);
        // }
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay of 10 milliseconds
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
