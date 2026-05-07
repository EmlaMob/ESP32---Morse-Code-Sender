#include "MENU_OLED.h"
#include <SETTING_TEXT.h>

#define UP 14
#define DOWN 12
#define ENTER 27
#define BACK 13

static int selected = 0;
static bool Refresh = true;
static int lastLogin = -2;

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

    if (lastLogin != LOGIN) {
        Refresh = true;
        lastLogin = LOGIN;
    };
    static unsigned long lastButtonTime = 0;
    if(millis() - lastButtonTime > 200){
        if (up == LOW) {
            selected = selected - 1;
            if (LOGIN == 0) {
                if(selected < 0) selected = 1;
            }
            if (selected < 0) selected = 2;
            lastButtonTime = millis();
        }

        else if (down == LOW) {
            selected = selected + 1;
            if (LOGIN == 0) {
                if(selected > 1) selected = 0;
            }
            if (selected > 2) selected = 0;
            lastButtonTime = millis();
        }

        else if (enter == LOW) {
            if(LOGIN == -1){
                LOGIN = selected;
                selected = 0;
            } else if(LOGIN == 0){
                if(selected == 0) LOGIN = 10;
                if(selected == 1) LOGIN = 11;
                selected = 0;
            }
            lastButtonTime = millis();
        }

        else if (back == LOW) {
            if(LOGIN < 10){
                LOGIN = -1;
                selected = 0;
            }
            lastButtonTime = millis();
        }
    }

    const char *options[3] = {
        " Transmit, Receive Morse",
        " Test Morse",
        " Study Morse"
    };

    if (LOGIN == -1) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(F("Select Mode:"));
        int currentY = 16;
        display.println("");

        for (int i = 0; i < 3; i++) {
        if (i == selected) {
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            printSlide(display, options[i], currentY);
        } else if (i != selected) {
            display.setTextWrap(false);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, currentY);
            display.print(options[i]);
            display.setTextWrap(true);
        }
        currentY += 16;
        }
        display.display();
    }   
    else if (LOGIN == 0) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(F("Choose Mode:"));
        display.println("");
        const char *subOptions[2] = {
            " Transmit Morse",
            " Receive Morse",
        };
        int currentY = 22;
        for(int i = 0; i < 2; i++){
            if(i == (selected % 2)) {
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            printSlide(display, subOptions[i], currentY);
            } else {
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, currentY);
            display.println(subOptions[i]);
            }
            currentY += 22;
        }
        display.display();
    }
}