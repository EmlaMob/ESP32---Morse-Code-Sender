#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "menu_oled.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

#define B_MORSE 25
unsigned long Press = 0;
bool isPress = false;
char currentMorse[10]; 
int morseIndex = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int LOGIN = -1;

void setup() {
    setupMenuPins();
    pinMode(B_MORSE, INPUT_PULLUP);
    Serial.begin(115200);
    Wire.setClock(400000);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.clearDisplay();
    display.setCursor(0,0);
    display.drawPixel(64,32,WHITE);
    display.display();
    delay(1000);
}

void loop() {
    displayMenu(display, LOGIN);

    if(LOGIN == 10){ 
        // MENU GO MORSE
        // NHAP CODE DE TEST VAO
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Sending:");
        display.println("");
        display.display();

        // PRINT RA MAN HINH
    }
    else if (LOGIN == 11) {
        // MENU NHAN MORSE
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Getting:");
        display.println("");  
        display.display();
    }
}