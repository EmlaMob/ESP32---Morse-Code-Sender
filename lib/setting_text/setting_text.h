#ifndef setting_text
#define setting_text

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

void printCenter(Adafruit_SSD1306 &display, String text, int y, int textSize);
void printDecodeText(Adafruit_SSD1306 &display, String text, int startY);

#endif