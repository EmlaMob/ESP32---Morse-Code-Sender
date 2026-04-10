#ifndef SETTING_TEXT_H
#define SETTING_TEXT_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

void printCenter(Adafruit_SSD1306 &display, const char* text, int y, int textSize);
void printDecodeText(Adafruit_SSD1306 &display, const char* text, int startY);

#endif