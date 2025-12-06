#include <Arduino.h>
#include <ESP32Servo.h>

#define TRIG 5
#define ECHO 18
#define SERVO_PIN 4

Servo radar;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  radar.attach(SERVO_PIN);
}

long getDistance() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  for (int a = 0; a <= 180; a += 5) {
    radar.write(a);
    delay(100);
    Serial.print("Angle: ");
    Serial.print(a);
    Serial.print(" Distance: ");
    Serial.println(getDistance());
  }
}
