void showYellow()
{
    rgb_led_1.setPixelColor(0, rgb_led_1.Color(255, 255, 51));
    rgb_led_1.show();
}

void showRed()
{
    rgb_led_1.setPixelColor(0, rgb_led_1.Color(255, 0, 0));
    rgb_led_1.show();
}

void showBlue()
{
    rgb_led_1.setPixelColor(0, rgb_led_1.Color(51, 102, 255));
    rgb_led_1.show();
}

void showGreen()
{
    rgb_led_1.setPixelColor(0, rgb_led_1.Color(51, 255, 51));
    rgb_led_1.show();
}


void smartBlink(uint8_t one, uint8_t two, uint8_t three)
{ 
  blink_startInterval = millis();
  while(millis() - blink_startInterval <= 5000){
    blink2_startInterval = millis();
    if (blink2_startInterval > blink_actualInterval + blinkInterval) {
      blink_actualInterval = millis();
      on = !on;
      if(on){
        rgb_led_1.setPixelColor(0,rgb_led_1.Color(0,0,0));
        rgb_led_1.show();
      }
      else {
        rgb_led_1.setPixelColor(0, rgb_led_1.Color(one, two, three));
        rgb_led_1.show();
      }
    }
  }
}