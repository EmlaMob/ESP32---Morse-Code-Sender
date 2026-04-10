#include "setting_text.h"


#define SCREEN_WIDTH 128

void printCenter(Adafruit_SSD1306 &display, String text, int y, int textSize) {
    int16_t x1, y1;
    uint16_t w, h;
    display.setTextSize(textSize);
    display.getTextBounds(text.c_str(), 0, 0, &x1, &y1, &w, &h);
    int cursorX = (SCREEN_WIDTH - w) / 2;
    display.setCursor(cursorX, y);
    display.println(text);
}

void printDecodeText(Adafruit_SSD1306 &display, String text, int startY) {
    if (text.length() == 0) return;
    int charWidthSize2 = 12; 
    int expectedWidth = text.length() * charWidthSize2;
    int textSize = 2;
    if (expectedWidth > SCREEN_WIDTH) {
        textSize = 1;
    }
    
    display.setTextSize(textSize);

    int charWidth = (textSize == 1) ? 6 : 12;
    int lineHeight = (textSize == 1) ? 8 : 16;
    
    int currentX = 0;
    int currentY = startY;

    int startIdx = 0;
    while (startIdx < text.length()) {
        int delimIdx = text.indexOf('/', startIdx);
        
        String word = "";
        String delim = "";

        if (delimIdx == -1) {
            word = text.substring(startIdx);
            startIdx = text.length();
        } else {
            word = text.substring(startIdx, delimIdx);
            delim = String(text.charAt(delimIdx));
            startIdx = delimIdx + 1;
        }

        int wordWidth = word.length() * charWidth;
        int delimWidth = delim.length() * charWidth;

        if (currentX + wordWidth > SCREEN_WIDTH && currentX > 0) {
            currentX = 0;
            currentY += lineHeight;
        }

        display.setCursor(currentX, currentY);
        display.print(word);
        currentX += wordWidth;

        if (delim != "") {
            if (currentX + delimWidth > SCREEN_WIDTH) {
                currentX = 0;
                currentY += lineHeight;
            }
            display.setCursor(currentX, currentY);
            display.print(delim);
            currentX += delimWidth;
        }
    }
}