/*
  Project: LED Mode Controller with OLED and Buzzer 
  Name: Umar Mushtaq
  Reg No: 23-NTU-CS-1077
  Date: 23-Oct-2025
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin definitions ---
#define LED1 2           // LED 1 pin
#define LED2 4           // LED 2 pin
#define LED3 5           // LED 3 (PWM LED)
#define BTN_MODE 26      // Button for changing modes
#define BTN_RESET 27     // Button for resetting to OFF
#define BUZZER 15        // Buzzer pin

// --- OLED display object ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// --- Variables for modes and LED control ---
volatile int mode = 0;                 // stores current LED mode (1–4)
volatile bool modeChanged = false;     // flag to indicate mode button pressed
volatile bool resetPressed = false;    // flag for reset button

unsigned long prevMillis = 0;          // for timing LED blink
bool ledState = false;                 // LED toggle flag

// --- Function: show message on OLED ---
void showMsg(String msg) {
  display.clearDisplay();          // clear old screen content
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.print("Mode: ");         // show "Mode:" label
  display.println(msg);            // print current mode
  display.display();               // update OLED screen
}

// --- Function: make a short beep sound ---
void beepBuzzer(int freq, int dur) {
  tone(BUZZER, freq, dur);         // start buzzer tone
  delay(dur + 50);                 // wait for duration
  noTone(BUZZER);                  // stop buzzer
}

// --- Interrupt Service Routine for MODE button ---
void IRAM_ATTR handleModeButton() {
  static unsigned long lastPress = 0;
  unsigned long currentMillis = millis();

  // Debounce check
  if (currentMillis - lastPress > 200) {
    modeChanged = true;
    lastPress = currentMillis;
  }
}

// --- Interrupt Service Routine for RESET button ---
void IRAM_ATTR handleResetButton() {
  static unsigned long lastPress = 0;
  unsigned long currentMillis = millis();

  // Debounce check
  if (currentMillis - lastPress > 200) {
    resetPressed = true;
    lastPress = currentMillis;
  }
}

void setup() {
  // --- Pin setup ---
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);   // use internal pull-up
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  // --- Initialize OLED display ---
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  showMsg("Both OFF");              // show default mode at start

  // --- Attach interrupts ---
  attachInterrupt(digitalPinToInterrupt(BTN_MODE), handleModeButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_RESET), handleResetButton, FALLING);
}

void loop() {
  // --- Handle Mode change interrupt ---
  if (modeChanged) {
    modeChanged = false;
    mode++;
    if (mode > 4) mode = 1;         // wrap back to mode 1 after 4

    // --- Change behavior based on selected mode ---
    switch (mode) {
      case 1:
        // Mode 1: Both LEDs OFF
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        showMsg("Both OFF");
        beepBuzzer(800, 120);       // short beep
        break;

      case 2:
        // Mode 2: Alternate Blink
        showMsg("Alternate Blink");
        beepBuzzer(1000, 120);
        break;

      case 3:
        // Mode 3: Both LEDs ON
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
        showMsg("Both ON");
        beepBuzzer(1200, 120);
        break;

      case 4:
        // Mode 4: PWM Fade
        showMsg("PWM Fade");
        beepBuzzer(1500, 120);
        break;
    }
  }

  // --- Handle Reset interrupt ---
  if (resetPressed) {
    resetPressed = false;
    mode = 1;                        // go back to mode 1 (OFF)
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    analogWrite(LED3, 0);
    showMsg("Reset to OFF");
    beepBuzzer(400, 200);
  }

  // --- Mode behaviors ---
  if (mode == 2) { // Alternate Blink mode
    if (millis() - prevMillis >= 500) {
      prevMillis = millis();
      ledState = !ledState;           // toggle state
      digitalWrite(LED1, ledState);
      digitalWrite(LED2, !ledState);
    }
  }

  if (mode == 4) { // PWM Fade mode
    // increase LED brightness
    for (int i = 0; i <= 255; i++) {
      analogWrite(LED3, i);
      delay(5);
    }
    // decrease LED brightness
    for (int i = 255; i >= 0; i--) {
      analogWrite(LED3, i);
      delay(5);
    }
  }
}
