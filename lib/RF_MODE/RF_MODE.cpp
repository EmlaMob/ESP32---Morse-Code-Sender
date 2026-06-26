#include "RF_MODE.h"
#include "MORSE_DICT.h"
#include "SETTING_TEXT.h"
#include <string.h>


void playMorseChar(char c, int LED_PIN, int BUZZER_PIN){
    const char *pattern = NULL;
    if (c >= 'A' && c <= 'Z')
        pattern = letters[c - 'A'];
    else if (c >= '0' && c <= '9')
        pattern = numbers[c - '0'];
    else if (c == '/'){
        delay(1750);
        return;
    }

    if (pattern){
        for (int i = 0; pattern[i] != '\0'; i++)
        {
            if (pattern[i] == '.'){
                digitalWrite(LED_PIN, HIGH);
                tone(BUZZER_PIN, 1000);
                delay(250);
            }
            else if (pattern[i] == '-'){
                digitalWrite(LED_PIN, HIGH);
                tone(BUZZER_PIN, 1000);
                delay(750);
            }
            noTone(BUZZER_PIN);
            digitalWrite(LED_PIN, LOW);
            delay(150);
        }
        delay(300);
    }
}

void runTransmitMode(Adafruit_SSD1306 &display, RF24 &radio, bool hasRadio, int &LOGIN, int B_MORSE, int ERASE, int LED_PIN, int BUZZER_PIN){
    static char sendCode[10] = "";
    static char sendText[100] = "";
    static unsigned long sendReleaseTime = 0;  
    static bool isSendPending = false;
    static bool isPress = false;
    static unsigned long Press = 0;
    static int lastlogin = -1;

    if (lastlogin != 10) {
        sendCode[0] = '\0';
        sendText[0] = '\0';
        isSendPending = false;
        isPress = false;
        lastlogin = 10;

        while(Serial.available() > 0) {
            Serial.read();
        } 
    }


    if (Serial.available() > 0) {
        char incomingChar = Serial.read();
        if (incomingChar >= 'a' && incomingChar <= 'z') incomingChar -= 32;
        bool isValid = (incomingChar >= 'A' && incomingChar <= 'Z') || (incomingChar >= '0' && incomingChar <= '9');
        if (incomingChar == ' ') {
            int textLen = strlen(sendText);
            if (textLen > 0 && textLen < 99 && sendText[textLen-1] != '/') {
                sendText[textLen] = '/';
                sendText[textLen + 1] = '\0';
                if (hasRadio) radio.write(&sendText, sizeof(sendText));
            }
        } 
        else if (isValid) {
            int textLen = strlen(sendText);
            if (textLen < 99) {
                sendText[textLen] = incomingChar;
                sendText[textLen + 1] = '\0';
                if (hasRadio) radio.write(&sendText, sizeof(sendText)); 
                playMorseChar(incomingChar, LED_PIN, BUZZER_PIN); 
                sendReleaseTime = millis(); 
                Serial.println(sendText);
            }
        }
    }
    radio.stopListening();
    int buttonState = digitalRead(B_MORSE);

    if(digitalRead(ERASE) == LOW) {
        if (strlen(sendText) > 0 || strlen(sendCode) > 0){
            sendText[0] = '\0';
            sendCode[0] = '\0';
            isSendPending = false;
            delay(200);
        }
        else
        {
            LOGIN = 0;
            lastlogin = 0;
            delay(200);
            return;
        }
    }

    if (buttonState == LOW && isPress == false){
        isPress = true;
        Press = millis();
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(30);
    }
    else if (buttonState == HIGH && isPress == true){
        isPress = false;
        digitalWrite(LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);

        unsigned long duration = millis() - Press;
        sendReleaseTime = millis();
        isSendPending = true;

        int len = strlen(sendCode);
        if (len < 9){
            if (duration > 20 && duration < 250){
                sendCode[len] = '.';
                sendCode[len + 1] = '\0';
            }
            else if (duration >= 250){
                sendCode[len] = '-';
                sendCode[len + 1] = '\0';
            }
        }
        delay(20);
    }

    unsigned long gap = millis() - sendReleaseTime;

    if (buttonState == HIGH && isSendPending == true){
        if (gap > 750 && strlen(sendCode) > 0){
            char decodedChar = '?';

            for (int i = 0; i < 26; i++){
                if (strcmp(sendCode, letters[i]) == 0)
                {
                    decodedChar = i + 'A';
                    break;
                }
            }
            if (decodedChar == '?'){
                for (int i = 0; i < 10; i++)
                {
                    if (strcmp(sendCode, numbers[i]) == 0)
                    {
                        decodedChar = i + '0';
                        break;
                    }
                }
            }
            int textLen = strlen(sendText);
            if (textLen < 99){
                sendText[textLen] = decodedChar;
                sendText[textLen + 1] = '\0';
                Serial.print("Current String: "); Serial.println(sendText);
                if (hasRadio) radio.write(&sendText, sizeof(sendText));
            }
            sendCode[0] = '\0';
            isSendPending = false;
        }
   }
        if(buttonState == HIGH && gap > 1750 && strlen(sendText) > 0 && sendText[strlen(sendText)-1] != '/'){
            int textLen = strlen(sendText);
            if(textLen < 99)
            {
                sendText[textLen] = '/';
                sendText[textLen + 1] = '\0';
                if (hasRadio) radio.write(&sendText, sizeof(sendText));
            }
            sendReleaseTime = millis();
        }
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.setTextColor(SSD1306_WHITE);
    display.println("Transmitting:");
    printCenter(display, sendCode, 10, 2);
    printDecodeText(display, sendText, 30);
    display.display();
}

void runReceiveMode(Adafruit_SSD1306 &display, RF24 &radio, bool hasRadio, int &LOGIN, int ERASE, int LED_PIN, int BUZZER_PIN){
    static char decodedText[100] = "";
    static int lastLen = 0;
    static int lastLogin = -1;
    if (lastLogin != 11){
        decodedText[0] = '\0';
        lastLen = 0;
        lastLogin = 11;
    }
    radio.startListening();
    if (digitalRead(ERASE) == LOW){
        if (strlen(decodedText) > 0)
        {
            decodedText[0] = '\0';
            lastLen = 0;
            delay(200);
        }
        else
        {
            LOGIN = 0;
            lastLogin = 0;
            delay(200);
            return;
        }
    }
    if (radio.available()){
        radio.read(&decodedText, sizeof(decodedText));
        Serial.println(decodedText);
        int currentLen = strlen(decodedText);
        if (currentLen > lastLen)
        {
            char newChar = decodedText[currentLen - 1];
            playMorseChar(newChar, LED_PIN, BUZZER_PIN);
            lastLen = currentLen;
        }
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.setTextColor(SSD1306_WHITE);
    display.println("Receiving:");
    printSlide(display, decodedText, 30);
    display.display();
}