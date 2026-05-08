#ifndef RF_MODE_H
#define RF_MODE_H

#include <Arduino.h>
#include <RF24.h>
#include <Adafruit_SSD1306.h>

void playMorseChar(char c, int LED_PIN, int BUZZER_PIN);
void runTransmitMode(Adafruit_SSD1306 &display, RF24 &radio, bool hasRadio, int &LOGIN, int B_MORSE, int ERASE, int LED_PIN, int BUZZER_PIN);
void runReceiveMode(Adafruit_SSD1306 &display, RF24 &radio, bool hasRadio, int &LOGIN, int pinErase, int pinLed, int pinBuzzer);

#endif