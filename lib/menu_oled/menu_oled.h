#ifndef MENU_OLED_H
#define MENU_OLED_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

void setupMenuPins();
void displayMenu(Adafruit_SSD1306 &display, int &LOGIN);

#endif