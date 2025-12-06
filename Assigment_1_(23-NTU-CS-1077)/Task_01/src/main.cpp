/*
  Project: LED Mode Controller with OLED and Buzzer (Interrupt + LEDC)
  Name: Umar Mushtaq
  Reg No: 23-NTU-CS-1077
  Date: 23-Oct-2025
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin definitions ---
#define LED1 2
#define LED2 4
#define LED3 5           // PWM LED (LEDC)
#define BTN_MODE 26
#define BTN_RESET 27
#define BUZZER 15

// --- OLED display ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// --- Variables ---
volatile int mode = 0;
volatile bool modeChanged = false;
volatile bool resetPressed = false;

unsigned long prevMillis = 0;
unsigned long fadeMillis = 0;
bool ledState = false;

// --- LEDC PWM parameters ---
const int pwmChannel = 0;
const int freq = 5000;        // 5 kHz
const int resolution = 8;     // 8-bit resolution (0–255)
int brightness = 0;
int fadeAmount = 5;

// --- OLED Display Message ---
void showMsg(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.print("Mode: ");
  display.println(msg);
  display.display();
}

// --- Beep buzzer ---
void beepBuzzer(int freq, int dur) {
  tone(BUZZER, freq, dur);
  delay(dur + 50);
  noTone(BUZZER);
}

// --- Interrupt Service Routines ---
void IRAM_ATTR handleModeButton() {
  static unsigned long lastPress = 0;
  unsigned long now = millis();
  if (now - lastPress > 200) {   // debounce
    modeChanged = true;
    lastPress = now;
  }
}

void IRAM_ATTR handleResetButton() {
  static unsigned long lastPress = 0;
  unsigned long now = millis();
  if (now - lastPress > 200) {   // debounce
    resetPressed = true;
    lastPress = now;
  }
}

void setup() {
  // Pin modes
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  showMsg("Both OFF");

  // --- LEDC PWM Setup ---
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(LED3, pwmChannel);
  ledcWrite(pwmChannel, 0);  // start with LED3 OFF

  // --- Attach Interrupts ---
  attachInterrupt(digitalPinToInterrupt(BTN_MODE), handleModeButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_RESET), handleResetButton, FALLING);
}

void loop() {
  // Handle mode button press
  if (modeChanged) {
    modeChanged = false;
    mode++;
    if (mode > 4) mode = 1;

    switch (mode) {
      case 1:
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        ledcWrite(pwmChannel, 0);
        showMsg("Both OFF");
        beepBuzzer(800, 100);
        break;

      case 2:
        showMsg("Alternate Blink");
        beepBuzzer(1000, 100);
        break;

      case 3:
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
        ledcWrite(pwmChannel, 0);
        showMsg("Both ON");
        beepBuzzer(1200, 100);
        break;

      case 4:
        showMsg("PWM Fade");
        beepBuzzer(1500, 100);
        break;
    }
  }

  // Handle reset
  if (resetPressed) {
    resetPressed = false;
    mode = 1;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    ledcWrite(pwmChannel, 0);
    showMsg("Reset to OFF");
    beepBuzzer(400, 200);
  }

  // --- Mode behaviors ---

  // Mode 2: Alternate Blink (non-blocking)
  if (mode == 2 && millis() - prevMillis >= 500) {
    prevMillis = millis();
    ledState = !ledState;
    digitalWrite(LED1, ledState);
    digitalWrite(LED2, !ledState);
  }

  // Mode 4: PWM Fade (non-blocking LEDC)
  if (mode == 4 && millis() - fadeMillis >= 20) {
    fadeMillis = millis();
    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 255) fadeAmount = -fadeAmount;
    ledcWrite(pwmChannel, brightness);
  }
}
