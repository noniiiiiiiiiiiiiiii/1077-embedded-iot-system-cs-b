#define BLYNK_TEMPLATE_ID "TMPL6UUfvwEMk"
#define BLYNK_TEMPLATE_NAME "ESP32  RADAR SYSTEM"
#define BLYNK_AUTH_TOKEN "dlm-9M7fS-MdeuJhvBjolPvOYRo7pj_L"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Ali Faisal";
char pass[] = "unavailable";

/*************** HARDWARE *****************/
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*************** PINS *****************/
#define TRIG_PIN    5
#define ECHO_PIN    18
#define SERVO_PIN   4

#define RED_LED     25
#define GREEN_LED   26

/*************** OLED *****************/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*************** OBJECTS *****************/
Servo radarServo;
BlynkTimer timer;

/*************** VARIABLES *****************/
bool radarON = false;

int servoAngle = 0;
int direction = 1;

int stepSize = 1;
unsigned long lastMove = 0;

long distanceCM = 0;

/*************** BLYNK *****************/
// Radar ON / OFF
BLYNK_WRITE(V0) {
  radarON = param.asInt();
}

// Speed Slider (10–100)
BLYNK_WRITE(V1) {
  int val = param.asInt();
  if (val <= 30) stepSize = 1;
  else if (val <= 50) stepSize = 2;
  else if (val <= 70) stepSize = 3;
  else if (val <= 85) stepSize = 4;
  else stepSize = 5;
}

/*************** ULTRASONIC *****************/
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 0;

  long dist = duration * 0.034 / 2;
  if (dist > 400) dist = 400;

  return dist;
}

/*************** LED LOGIC *****************/
String ledStatus = "";

void updateLEDs(long dist) {
  if (dist > 0 && dist < 40) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    ledStatus = "RED (NEAR)";
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    ledStatus = "GREEN (SAFE)";
  }
}

/*************** OLED *****************/
void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print("Radar: ");
  display.println(radarON ? "ON" : "OFF");

  display.setCursor(0, 14);
  display.print("Angle: ");
  display.print(servoAngle);
  display.println(" deg");

  display.setCursor(0, 28);
  display.print("Dist: ");
  display.print(distanceCM);
  display.println(" cm");

  display.setCursor(0, 42);
  display.print("LED: ");
  display.println(ledStatus);

  if (distanceCM > 0 && distanceCM < 20) {
    display.setCursor(0, 56);
    display.println("WARNING!");
  }

  display.display();
}

/*************** RADAR TASK *****************/
void radarTask() {

  if (!radarON) {
    radarServo.write(0);
    updateOLED();
    return;
  }

  // Smooth Servo Motion
  if (millis() - lastMove >= 25) {
    lastMove = millis();
    servoAngle += direction * stepSize;

    if (servoAngle >= 180) {
      servoAngle = 180;
      direction = -1;
    }
    if (servoAngle <= 0) {
      servoAngle = 0;
      direction = 1;
    }
    radarServo.write(servoAngle);
  }

  // Distance + LEDs
  distanceCM = getDistance();
  updateLEDs(distanceCM);

  // Blynk Updates
  Blynk.virtualWrite(V2, servoAngle);
  Blynk.virtualWrite(V3, distanceCM);

  // Serial Monitor
  Serial.print("Angle: ");
  Serial.print(servoAngle);
  Serial.print(" | Distance: ");
  Serial.print(distanceCM);
  Serial.print(" | LED: ");
  Serial.println(ledStatus);

  updateOLED();
}

/*************** SETUP *****************/
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  radarServo.attach(SERVO_PIN, 500, 2400);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(50L, radarTask);
}

/*************** LOOP *****************/
void loop() {
  Blynk.run();
  timer.run();
}
