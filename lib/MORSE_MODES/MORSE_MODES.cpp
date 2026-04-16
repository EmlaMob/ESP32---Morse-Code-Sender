#include "MORSE_MODES.h"
#include "SETTING_TEXT.h"
#include <string.h>
#include "MORSE_DICT.h"

static int testQuizIndex = -1;  
static int studyQuizIndex = -1;

static char currentInput[10] = "";
static unsigned long releaseTime = 0;
static bool isAwaitingInput = true;
static bool isPress = false;
static unsigned long pressTime = 0;


void resetQuiz() {
    currentInput[0] = '\0';
    isAwaitingInput = true;
    isPress = false;
}

void runTestMode(Adafruit_SSD1306 &display, int &LOGIN, int pinMorse, int pinErase, int pinLed, int pinBuzzer) {
    if(testQuizIndex == -1){
        testQuizIndex = esp_random() % numQuizChars;
    }
    
    int buttonState = digitalRead(pinMorse);


    if (digitalRead(pinErase) == LOW) {
        testQuizIndex = esp_random() % numQuizChars;
        resetQuiz();
        LOGIN = -1; 
        delay(200);
        return;
    }


    if (isAwaitingInput) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("Testing:");

        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(55, 10);
        display.print(quizChars[testQuizIndex]);

        display.setTextSize(1);
        display.setCursor(0, 40);
        display.print("You entered: ");
        display.println(currentInput);
        display.display();
    }


    if (buttonState == LOW && isPress == false) {
        isPress = true;
        pressTime = millis();
        digitalWrite(pinLed, HIGH);
        digitalWrite(pinBuzzer, HIGH);
        delay(20);
    } 
    else if (buttonState == HIGH && isPress == true) {
        isPress = false;
        digitalWrite(pinLed, LOW);
        digitalWrite(pinBuzzer, LOW);
        unsigned long duration = millis() - pressTime;
        releaseTime = millis();
        isAwaitingInput = true;

        int len = strlen(currentInput);
        if (len < 9) {
            if (duration > 20 && duration < 250) {
                currentInput[len] = '.';
                currentInput[len + 1] = '\0';
            } else if (duration >= 250) {
                currentInput[len] = '-';
                currentInput[len + 1] = '\0';
            }
        }
        delay(20);
    }


    if (strlen(currentInput) > 0 && (millis() - releaseTime > 1500)) {
        int letterIndex = quizChars[testQuizIndex] - 'A';
        
        display.clearDisplay();
        if (strcmp(currentInput, letters[letterIndex]) == 0) {
            printCenter(display, "Correct!", 20, 2);
            int newIndex = testQuizIndex;
            while(newIndex == testQuizIndex) {
                newIndex = esp_random() % numQuizChars;
            }
            testQuizIndex = newIndex;

        } else {
            printCenter(display, "Incorrect!", 20, 2);
        }
        display.display();
        delay(1500);
        
        currentInput[0] = '\0';
    }
}


void runStudyMode(Adafruit_SSD1306 &display, int &LOGIN, int pinMorse, int pinErase, int pinLed, int pinBuzzer) {
    if(studyQuizIndex == -1){
        studyQuizIndex = esp_random() % numQuizChars;
    }
    
    int buttonState = digitalRead(pinMorse);

    if (digitalRead(pinErase) == LOW) {
        studyQuizIndex = esp_random() % numQuizChars;
        resetQuiz();
        LOGIN = -1; 
        delay(200);
        return;
    }

    if (isAwaitingInput) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("Studying:");

        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(55, 12);
        display.print(quizChars[studyQuizIndex]);

        display.setTextSize(1);
        int letterIndex = quizChars[studyQuizIndex] - 'A';
        display.setCursor(0, 35);
        display.print("Enter: ");
        display.setCursor(40, 35);
        display.println(letters[letterIndex]);

        display.setCursor(0, 50);
        display.print("You entered: ");
        display.setCursor(75, 50);
        display.println(currentInput);
        display.display();
    }


    if (buttonState == LOW && isPress == false) {
        isPress = true;
        pressTime = millis();
        digitalWrite(pinLed, HIGH);
        digitalWrite(pinBuzzer, HIGH);
        delay(20);
    } 
    else if (buttonState == HIGH && isPress == true) {
        isPress = false;
        digitalWrite(pinLed, LOW);
        digitalWrite(pinBuzzer, LOW);
        unsigned long duration = millis() - pressTime;
        releaseTime = millis();
        isAwaitingInput = true;

        int len = strlen(currentInput);
        if (len < 9) {
            if (duration > 20 && duration < 250) {
                currentInput[len] = '.';
                currentInput[len + 1] = '\0';
            } else if (duration >= 250) {
                currentInput[len] = '-';
                currentInput[len + 1] = '\0';
            }
        }
        delay(20);
    }

    if (strlen(currentInput) > 0 && (millis() - releaseTime > 1500)) {
        int letterIndex = quizChars[studyQuizIndex] - 'A';
        
        display.clearDisplay();
        if (strcmp(currentInput, letters[letterIndex]) == 0) {
            printCenter(display, "GOOD!", 20, 2);
            int newIndex = studyQuizIndex;
            while(newIndex == studyQuizIndex) {
                newIndex = esp_random() % numQuizChars;
            }
            studyQuizIndex = newIndex;

        } else {
            printCenter(display, "Incorrect!", 20, 2);
        }
        display.display();
        delay(1500);
        
        currentInput[0] = '\0'; 
    }
}