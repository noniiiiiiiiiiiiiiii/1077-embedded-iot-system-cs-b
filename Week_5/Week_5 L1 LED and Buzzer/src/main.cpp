// === ESP32: Two LEDs + One Buzzer (All Separate PWM Channels) ===
// LEDs fade continuously; buzzer plays pattern repeatedly in loop()
// -----------------------------------------------------------------

#include <Arduino.h>

// --- LED 1 ---
#define LED1_PIN 18
#define LED1_CH  0
#define LED_FREQ 5000
#define LED_RES  8  // 8-bit = 0–255

// --- LED 2 ---
#define LED2_PIN 19
#define LED2_CH  1
#define LED2_FREQ 5000
#define LED2_RES 8

// --- Buzzer ---
#define BUZZER_PIN 27
#define BUZZ_CH    2   // Third channel
#define BUZZ_FREQ  2000
#define BUZZ_RES   10  // 10-bit = 0–1023

void setup() {
  // --- LED1 setup ---
  ledcSetup(LED1_CH, LED_FREQ, LED_RES);
  ledcAttachPin(LED1_PIN, LED1_CH);

  // --- LED2 setup ---
  ledcSetup(LED2_CH, LED2_FREQ, LED2_RES);
  ledcAttachPin(LED2_PIN, LED2_CH);

  // --- Buzzer setup ---
  ledcSetup(BUZZ_CH, BUZZ_FREQ, BUZZ_RES);
  ledcAttachPin(BUZZER_PIN, BUZZ_CH);
}

void loop() {
  // --- LED Fade Effect ---
  for (int d = 0; d <= 255; d++) {
    ledcWrite(LED1_CH, d);         // LED1 brightening
    ledcWrite(LED2_CH, 255 - d);   // LED2 dimming
    delay(10);
  }
  for (int d = 255; d >= 0; d--) {
    ledcWrite(LED1_CH, d);
    ledcWrite(LED2_CH, 255 - d);
    delay(10);
  }

  // --- BUZZER Operations (Repeat every loop) ---

  // 1. Simple beep pattern
  for (int i = 0; i < 3; i++) {
    ledcWriteTone(BUZZ_CH, 2000 + i * 400);
    delay(150);
    ledcWrite(BUZZ_CH, 0);
    delay(150);
  }

  // 2. Frequency sweep
  for (int f = 400; f <= 3000; f += 100) {
    ledcWriteTone(BUZZ_CH, f);
    delay(20);
  }
  ledcWrite(BUZZ_CH, 0);
  delay(300);

  // 3. Short melody
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (int i = 0; i < 8; i++) {
    ledcWriteTone(BUZZ_CH, melody[i]);
    delay(200);
  }
  ledcWrite(BUZZ_CH, 0); // Stop buzzer

  delay(500); // pause before repeating loop
}
