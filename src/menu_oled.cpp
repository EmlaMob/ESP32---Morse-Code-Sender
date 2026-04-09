#include "menu_oled.h"

#define UP 14
#define DOWN 12
#define ENTER 27
#define BACK 13

static int selected = 0;

void setupMenuPins(){
    pinMode(DOWN, INPUT_PULLUP);
    pinMode(UP, INPUT_PULLUP);
    pinMode(ENTER, INPUT_PULLUP);
    pinMode(BACK, INPUT_PULLUP);
}

void displayMenu(Adafruit_SSD1306 &display, int &LOGIN){
    int down = digitalRead(DOWN);
    int up = digitalRead(UP);
    int enter = digitalRead(ENTER);
    int back = digitalRead(BACK);
    if (up == LOW) {
        selected = selected - 1;
        if(selected < 0) selected = 2;
        delay(200);
    };
    if (down == LOW) {
        selected = selected + 1;
        if(selected > 2) selected = 0;
        delay(200);
        };
    if (enter == LOW) {
        if(LOGIN == -1){
            LOGIN = selected;
            selected = 0;
        } else if(LOGIN == 0){
            if(selected == 0) LOGIN = 10;
            if(selected == 1) LOGIN = 11;
            selected = 0;
        }
        delay(200);
    };
    if (back == LOW) {
        if(LOGIN >= 10){
            LOGIN = 0;
        } else {
            LOGIN = -1;
        }
        selected = 0;
        delay(200);
    };
    const char *options[3] = {
        " Send, get Morse",
        " Test Morse",
        " Study Morse"
    };
    if (LOGIN == -1) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(F("Choose Mode:"));
        display.println("");
        for (int i = 0; i < 3; i++) {
        if (i == selected) {
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.println(options[i]);
        } else if (i != selected) {
            display.setTextColor(SSD1306_WHITE);
            display.println(options[i]);
        }
        display.println("");
        }
        display.display();
    }
    else if (LOGIN == 0) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(F("Send/Get:"));
        display.println("");
        const char *subOptions[2] = {
            " Send Morse",
            " Get Morse",
        };
        for(int i = 0; i < 2; i++){
            if(i == (selected % 2)) {
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.println(subOptions[i]);
            } else {
            display.setTextColor(SSD1306_WHITE);
            display.println(subOptions[i]);
            }
            display.println("");
        }
        display.display();
    }
    else if (LOGIN == 1) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(F("Testing:"));
        display.println("");
        display.display();
    } else if (LOGIN == 2){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(F("Studying:"));
        display.println("");
        display.display();
    }
}