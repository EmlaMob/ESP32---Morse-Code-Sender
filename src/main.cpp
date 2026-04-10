#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MENU_OLED.h>
#include <SETTING_TEXT.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define LED_PIN 18
#define BUZZER_PIN 19
#define B_MORSE 25
#define ERASE 13

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int LOGIN = -1;

unsigned long Press = 0;

bool isPress = false;
char sendSeq[10] = "";
char sendText[100] = "";
unsigned long sendReleaseTime = 0; 
bool isSendPending = false;


char getSeq[10] = "";
char decodedText[100] = "";
unsigned long getRealseTime = 0;
bool isGetPending = false;

const char* letters[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};
const char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-",
  ".....", "-....", "--...", "---..", "----."
};


void setup() {
    setupMenuPins();
    pinMode(ERASE, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
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
        int buttonState = digitalRead(B_MORSE);
        if(digitalRead(ERASE) == LOW){
            if(strlen(sendText) > 0 || strlen(sendSeq) > 0){
                sendText[0] = '\0';
                sendSeq[0] = '\0';
                isSendPending = false;
                delay(200);
            }   else {
                LOGIN = 0;
                delay(200);
            }
        }
        if(buttonState == LOW && isPress == false){
            isPress = true;
            Press = millis();
            digitalWrite(LED_PIN, HIGH);
            digitalWrite(BUZZER_PIN, HIGH);
            delay(20);
        }
        else if(buttonState == HIGH && isPress == true){
            isPress = false;
            digitalWrite(LED_PIN, LOW);
            digitalWrite(BUZZER_PIN, LOW);
            unsigned long duration = millis() - Press;
            sendReleaseTime = millis();
            isSendPending = true;

            int len = strlen(sendSeq);
            if(len < 9) {
                if(duration > 20 && duration < 250) {
                    sendSeq[len] = '.';
                    sendSeq[len+1] = '\0';
                }
                else if(duration >= 250) {
                    sendSeq[len] = '-';
                    sendSeq[len+1] = '\0';
                }
            }
            delay(20);
        }
        unsigned long gap = millis() - sendReleaseTime;
        if(buttonState == HIGH && isSendPending == true) {
            if(gap > 750 && sendSeq != ""){
                char decodedChar = '?';

                for(int i = 0; i < 26; i++) {
                    if(strcmp(sendSeq, letters[i]) == 0){
                        decodedChar = i + 'A';
                        break; 
                    }
                }
                if(decodedChar == '?') {
                    for(int i = 0; i < 10; i++) {
                        if(strcmp(sendSeq, numbers[i]) == 0){
                            decodedChar = i + '0';
                            break;
                        }
                    }
                }
                int textLen = strlen(sendText);
                if(textLen < 99) {
                    sendText[textLen] = decodedChar;
                    sendText[textLen+1] = '\0';
                }
                sendSeq[0] = '\0';
                isSendPending = false;
            }
        }
        if(buttonState == HIGH && gap > 2000 && strlen(sendText) > 0 && sendText[strlen(sendText) - 1] != '/'){
            int textLen = strlen(sendText);
            if(textLen < 99){
                sendText[textLen] = '/';
                sendText[textLen+1] = '\0';
            }
            sendReleaseTime = millis();
        }
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Sending:");
        printCenter(display, sendSeq, 10, 2);
        printDecodeText(display, sendText, 30);
        display.display();
    }
    else if (LOGIN == 11) {
        int buttonState = digitalRead(B_MORSE);
        if(digitalRead(ERASE) == LOW){
            if(strlen(decodedText) > 0 || strlen(getSeq) > 0){
                decodedText[0] = '\0';
                getSeq[0] = '\0';
                isGetPending = false;
                delay(200);
            }   else {
                LOGIN = 0;
                delay(200);
            }
        }
        if(buttonState == LOW && isPress == false){
            isPress = true;
            Press = millis();
            digitalWrite(LED_PIN, HIGH);
            digitalWrite(BUZZER_PIN, HIGH);
            delay(20);
        }
        else if(buttonState == HIGH && isPress == true){
            isPress = false;
            digitalWrite(LED_PIN, LOW);
            digitalWrite(BUZZER_PIN, LOW);
            unsigned long duration = millis() - Press;
            getRealseTime = millis();
            isGetPending = true;

            int len = strlen(getSeq);
            if(len < 9) {
                if(duration > 20 && duration < 250) {
                    getSeq[len] = '.';
                    getSeq[len+1] = '\0';
                }
                else if(duration >= 250) {
                    getSeq[len] = '-';
                    getSeq[len+1] = '\0';
                }
            }
            delay(20);
        }
        unsigned long gap = millis() - getRealseTime;
        if(buttonState == HIGH && isGetPending == true) {
            if(gap > 750 && strlen(getSeq) > 0){
                char decodedChar = '?';

                for(int i = 0; i < 26; i++) {
                    if(strcmp(getSeq, letters[i]) == 0){
                        decodedChar = i + 'A';
                        break; 
                    }
                }
                if(decodedChar == '?') {
                    for(int i = 0; i < 10; i++) {
                        if(strcmp(getSeq, numbers[i]) == 0){
                            decodedChar = i + '0';
                            break;
                        }
                    }
                }
                int textLen = strlen(decodedText);
                if(textLen < 99) {
                    decodedText[textLen] = decodedChar;
                    decodedText[textLen+1] = '\0';
                }
                getSeq[0] = '\0';
                isGetPending = false;
            }
        }
        if(buttonState == HIGH && gap > 2000 && strlen(decodedText) > 0 && decodedText[strlen(decodedText) - 1] != '/'){
            int textLen = strlen(decodedText);
            if(textLen < 99){
                decodedText[textLen] = '/';
                decodedText[textLen+1] = '\0';
            }
            getRealseTime = millis();
        }
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Getting:");
        printCenter(display, getSeq, 10, 2);
        printDecodeText(display, decodedText, 30);
        display.display();
    }
    if(LOGIN != 10){
        sendSeq[0] = '\0';
        sendText[0] = '\0';
        isSendPending = false;
    }
    if(LOGIN != 11) {
        getSeq[0] = '\0';
        decodedText[0] = '\0';
        isGetPending = false;
    }
}