#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <menu_oled.h>
#include <setting_text.h>

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
String sendSeq = "";
String sendText = "";
unsigned long sendReleaseTime = 0; 

bool isSendPending = false;
String getSeq = "";
String decodedText = "";
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
            if(sendText.length() > 0 || sendSeq.length() > 0){
                sendText = "";
                sendSeq = "";
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
            if(duration > 20 && duration < 250) {
                sendSeq += ".";
            }
            else if(duration >= 250) {
                sendSeq += "-";
            }
            delay(20);
        }
        unsigned long gap = millis() - sendReleaseTime;
        if(buttonState == HIGH && isSendPending == true) {
            if(gap > 750 && sendSeq != ""){
                char decodedChar = '?';
                for(int i = 0; i < 26; i++) {
                    if(sendSeq == letters[i]){
                        decodedChar = i + 'A';
                        break; 
                    }
                }
                if(decodedChar == '?') {
                    for(int i = 0; i < 10; i++) {
                        if(sendSeq == numbers[i]){
                            decodedChar = i + '0';
                            break;
                        }
                    }
                }
                sendText += decodedChar;
                sendSeq = "";
                isSendPending = false;
            }
        }
        if(buttonState == HIGH && gap > 2000 && sendText.length() > 0 && sendText.charAt(sendText.length()-1) != '/'){
            sendText += "/";
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
            if(duration > 20 && duration <= 250){
                getSeq += ".";
            }   else if(duration > 250) {
                getSeq += '-';
            }
            delay(20);
        }

        unsigned long gap = millis() - getRealseTime;
        if(buttonState == HIGH && isGetPending == true) {
            if(gap > 750 && getSeq != ""){
                char decodedChar = '?';
                for(int i = 0; i < 26; i++) {
                    if(getSeq == letters[i]){
                        decodedChar = i + 'A';
                        break; 
                    }
                }
                if(decodedChar == '?') {
                    for(int i = 0; i < 10; i++) {
                        if(getSeq == numbers[i]){
                            decodedChar = i + '0';
                            break;
                        }
                    }
                }
                decodedText += decodedChar;
                getSeq = "";
                isGetPending = false;
            }
        }
        if(buttonState == HIGH && gap > 1500 && decodedText.length() > 0 && decodedText.charAt(decodedText.length()-1) != ' '){
            decodedText += " ";
            getRealseTime = millis();
        }

        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.setTextColor(SSD1306_WHITE);
        display.println("Getting:");
        display.println("");
        display.println(getSeq);
        display.println(decodedText);
        display.display();
    }
    if(LOGIN != 10){
        sendSeq ="";
        sendText ="";
        isSendPending = false;
    }
    if(LOGIN != 11) {
        getSeq = "";
        decodedText = "";
        isGetPending = false;
    }
}