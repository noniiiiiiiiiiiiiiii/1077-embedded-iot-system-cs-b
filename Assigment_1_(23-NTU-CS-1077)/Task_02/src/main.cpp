/*
  Project: Button Press Detection (Short/Long Press) 
  Reg No: 23-NTU-CS-1077
  Date: 19-Oct-2025
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin Definitions ---
#define BTN 25        // Button pin
#define LED 5        // LED pin
#define BUZZER 18     // Buzzer pin

// --- OLED Display Setup (I2C) ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// --- Variables ---
volatile bool buttonPressed = false;       // Flag from interrupt
unsigned long pressStartTime = 0;          // When button pressed
bool ledState = false;                     // LED state

// --- Function to show text on OLED ---
void showText(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.println(msg);
  display.display();
}

// --- Interrupt Service Routine for Button Press ---
void IRAM_ATTR handleButton() {
  static unsigned long lastInterrupt = 0;
  unsigned long currentTime = millis();

  // --- Simple debounce ---
  if (currentTime - lastInterrupt > 200) {
    buttonPressed = true;          // Set flag for main loop
    lastInterrupt = currentTime;
  }
}

void setup() {
  pinMode(BTN, INPUT_PULLUP);      // Button input with internal pull-up
  pinMode(LED, OUTPUT);            // LED output
  pinMode(BUZZER, OUTPUT);         // Buzzer output

  // --- OLED Initialization ---
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showText("Ready...");

  // --- Attach interrupt (only on button press) ---
  attachInterrupt(digitalPinToInterrupt(BTN), handleButton, FALLING);
}

void loop() {
  // --- When button interrupt occurs ---
  if (buttonPressed) {
    buttonPressed = false;

    // Wait until button is released to measure duration
    pressStartTime = millis();
    while (digitalRead(BTN) == LOW) {
      delay(10); // wait for release
    }
    unsigned long pressDuration = millis() - pressStartTime;

    // --- Long Press Detection (>1.5 sec) ---
    if (pressDuration > 1500) {
      tone(BUZZER, 1000, 500);
      showText("Long Press → Buzzer");
       delay(1000);  //  Hold message for 1 sec
      noTone(BUZZER);
    }
    // --- Short Press Detection ---
    else {
      ledState = !ledState;
      digitalWrite(LED, ledState);
      showText("Short Press → LED Toggle");
    }
  }
}
