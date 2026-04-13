#ifndef MORSE_MODES_H
#define MORSE_MODES_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

void runTestMode(Adafruit_SSD1306 &display, int &LOGIN, int pinMorse, int pinErase, int pinLed, int pinBuzzer);
void runStudyMode(Adafruit_SSD1306 &display, int &LOGIN, int pinMorse, int pinErase, int pinLed, int pinBuzzer);

#endif