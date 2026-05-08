#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MENU_OLED.h>
#include <SETTING_TEXT.h>
#include <string.h>
#include <MORSE_MODES.h>
#include <RF24.h>
#include <printf.h>
#include <SPI.h>
#include <MORSE_DICT.h>
#include <RF_MODE.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define LED_PIN 16
#define BUZZER_PIN 17
#define B_MORSE 25
#define ERASE 13

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int LOGIN = -1;
RF24 radio(4, 5);
const byte address[6] = "00001";
bool hasRadio = false;


void setup()
{
    Serial.begin(115200);
    setupMenuPins();
    pinMode(ERASE, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(B_MORSE, INPUT_PULLUP);

    if (!radio.begin())
    {
        Serial.println(F("radio hardware not responding."));
        hasRadio = false;
    }
    else
    {
        hasRadio = true;
        radio.setPALevel(RF24_PA_LOW);
        radio.setDataRate(RF24_1MBPS);
        radio.setChannel(124);
        radio.openWritingPipe(address);
        radio.openReadingPipe(1, address);
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.display();
}

void loop(){
    displayMenu(display, LOGIN);

    if (LOGIN == 10){
        runTransmitMode(display, radio, hasRadio, LOGIN, B_MORSE, ERASE, LED_PIN, BUZZER_PIN);
    }
    else if (LOGIN == 11){
        runReceiveMode(display, radio, hasRadio, LOGIN, ERASE, LED_PIN, BUZZER_PIN);
    }
    else if (LOGIN == 1){
        runTestMode(display, LOGIN, B_MORSE, ERASE, LED_PIN, BUZZER_PIN);
    }
    else if (LOGIN == 2){
        runStudyMode(display, LOGIN, B_MORSE, ERASE, LED_PIN, BUZZER_PIN);
    }
}