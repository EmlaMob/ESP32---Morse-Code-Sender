#include "SETTING_TEXT.h"
#include <string.h>

#define SCREEN_WIDTH 128

void printCenter(Adafruit_SSD1306 &display, const char* text, int y, int textSize) {
    int16_t x1, y1;
    uint16_t w, h;
    display.setTextSize(textSize);
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    int cursorX = (SCREEN_WIDTH - w) / 2;
    display.setCursor(cursorX, y);
    display.println(text);
}

void printDecodeText(Adafruit_SSD1306 &display, const char* text, int startY) {
    int len = strlen(text);
    if(text == 0) return;

    int charWidthSize2 = 12;
    int textSize = (len * charWidthSize2 > SCREEN_WIDTH) ? 1 : 2;

    display.setTextSize(textSize);
    int charWidth = (textSize == 1) ? 6 : 12;
    int lineHeight = (textSize == 1) ? 8 : 16;
    
    int currentX = 0;
    int currentY = startY;
    char wordBuffer[20];

    int startIdx = 0;
    while (startIdx < len) {
        const char* slashPtr = strchr(text + startIdx, '/');
        int delimIdx = (slashPtr) ? (slashPtr - text) : -1;

        int wordLen;
        bool hasDelim = false;

        if (delimIdx == -1) {
            wordLen = len - startIdx;
        } else {
            wordLen = delimIdx - startIdx;
            hasDelim = true;
        }

        strncpy(wordBuffer, text + startIdx, wordLen);
        wordBuffer[wordLen] = '\0';

        if (currentX + (wordLen * charWidth) > SCREEN_WIDTH && currentX > 0) {
            currentX = 0;
            currentY += lineHeight;
        }

        display.setCursor(currentX, currentY);
        display.print(wordBuffer);
        currentX += (wordLen * charWidth);

        if (hasDelim) {
            if (currentX + charWidth > SCREEN_WIDTH) {
                currentX = 0;
                currentY += lineHeight;
            }
            display.setCursor(currentX, currentY);
            display.print("/");
            currentX += charWidth;
            startIdx = delimIdx + 1;
        }   else {
            startIdx = len;
        }
    }
}

void printSlide(Adafruit_SSD1306 &display, const char* text, int y){
    int16_t x1, y1;
    uint16_t w, h;
    display.setTextWrap(false);
    display.setTextSize(1);
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    if(w <= SCREEN_WIDTH){
        display.setCursor(0, y);
        display.print(text);
        display.setTextWrap(true);
        return;
    }
    
    static int16_t scrollX = 0;
    static unsigned long lastScrollTime = 0;
    static char lastText[100] = "";

    if (strcmp(text, lastText) != 0) {
        strncpy(lastText, text, 99);
        lastText[99] = '\0';
        scrollX = 0; 
    }
    
    int loopWidth = w;

    if (millis() - lastScrollTime > 75) { 
        scrollX -= 1;
        if (scrollX <= -loopWidth) {
            scrollX += loopWidth; 
        }
        lastScrollTime = millis();
    }
    display.setCursor(scrollX, y);
    display.print(text);
    
    display.setCursor(scrollX + loopWidth, y);
    display.print(text);

    display.setTextWrap(true);
}
