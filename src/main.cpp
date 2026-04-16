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

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

#define LED_PIN 16
#define BUZZER_PIN 17
#define B_MORSE 25
#define ERASE 13

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int LOGIN = -1;

unsigned long Press = 0;

bool isPress = false;

char sendCode[10] = "";
char sendText[100] = "";
unsigned long sendReleaseTime = 0; 
bool isSendPending = false;

char decodedText[100] = "";


RF24 radio(4, 5);
const byte address[6] = {"00001"};
bool hasRadio = false;

void playMorseChar(char c) {
    const char* pattern = NULL;
    if (c >= 'A' && c <= 'Z') pattern = letters[c - 'A'];
    else if (c >= '0' && c <= '9') pattern = numbers[c - '0'];
    else if (c == '/') { delay(1750); return; }

    if (pattern) {
        for (int i = 0; pattern[i] != '\0'; i++) {
            digitalWrite(LED_PIN, HIGH);
            digitalWrite(BUZZER_PIN, HIGH);
            if (pattern[i] == '.') delay(250);
            else if (pattern[i] == '-') delay(750);
            
            digitalWrite(LED_PIN, LOW);
            digitalWrite(BUZZER_PIN, LOW);
            delay(150);
        }
        delay(300);
    }
}

void setup() {
    setupMenuPins();
    pinMode(ERASE, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(B_MORSE, INPUT_PULLUP);


    if(!radio.begin()){
        Serial.println(F("radio hardware not responding."));
        hasRadio = false;
    }
    else {
        radio.setPALevel(RF24_PA_HIGH);
        radio.setDataRate(RF24_1MBPS);
        radio.setChannel(124);

        radio.openWritingPipe(address);
        radio.openReadingPipe(1, address);
    }
    


    Serial.begin(115200);
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
        radio.stopListening();
        int buttonState = digitalRead(B_MORSE);
        if(digitalRead(ERASE) == LOW){
            if(strlen(sendText) > 0 || strlen(sendCode) > 0){
                sendText[0] = '\0';
                sendCode[0] = '\0';
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

            int len = strlen(sendCode);
            if(len < 9) {
                if(duration > 20 && duration < 250) {
                    sendCode[len] = '.';
                    sendCode[len+1] = '\0';
                }
                else if(duration >= 250) {
                    sendCode[len] = '-';
                    sendCode[len+1] = '\0';
                }
            }
            delay(20);
        }

        unsigned long gap = millis() - sendReleaseTime;
        if(buttonState == HIGH && isSendPending == true) {
            if(gap > 750 && strlen(sendCode) > 0){
                char decodedChar = '?';

                for(int i = 0; i < 26; i++) {
                    if(strcmp(sendCode, letters[i]) == 0){
                        decodedChar = i + 'A';
                        break; 
                    }
                }
                if(decodedChar == '?') {
                    for(int i = 0; i < 10; i++) {
                        if(strcmp(sendCode, numbers[i]) == 0){
                            decodedChar = i + '0';
                            break;
                        }
                    }
                }
                int textLen = strlen(sendText);
                if(textLen < 99) {
                    sendText[textLen] = decodedChar;
                    sendText[textLen+1] = '\0';
                    if (hasRadio) { 
                        radio.write(&sendText, sizeof(sendText));
                    }
                }
                sendCode[0] = '\0';
                isSendPending = false;
            }
        }

        if(buttonState == HIGH && gap > 1750 && strlen(sendText) > 0 && sendText[strlen(sendText) - 1] != '/'){
            int textLen = strlen(sendText);
            if(textLen < 99){
                sendText[textLen] = '/';
                sendText[textLen+1] = '\0';
                if (hasRadio) { 
                    radio.write(&sendText, sizeof(sendText));
                }
            }
            sendReleaseTime = millis();
        }
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Transmitting:");
        printCenter(display, sendCode, 10, 2);
        printDecodeText(display, sendText, 30);
        display.display();
    }
    else if (LOGIN == 11) {
        radio.startListening();
        static int lastLen = 0;
        if(digitalRead(ERASE) == LOW){
            if(strlen(decodedText) > 0){
                decodedText[0] = '\0';
                lastLen = 0;
                delay(200);
            }   else {
                LOGIN = 0;
                delay(200);
            }
        }
        if(radio.available()){
            radio.read(&decodedText, sizeof(decodedText));
            int currentLen = strlen(decodedText);
            if(currentLen > lastLen){
                char newChar = decodedText[currentLen-1];
                playMorseChar(newChar);
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
    else if (LOGIN == 1) {
        runTestMode(display, LOGIN, B_MORSE, ERASE, LED_PIN, BUZZER_PIN);
    }
    else if (LOGIN == 2) {
        runStudyMode(display, LOGIN, B_MORSE, ERASE, LED_PIN, BUZZER_PIN);
    }
    if(LOGIN != 10){
        sendCode[0] = '\0';
        sendText[0] = '\0';
        isSendPending = false;
    }
    if(LOGIN != 11) {
        decodedText[0] = '\0';
    }
}