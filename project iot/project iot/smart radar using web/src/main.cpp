#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/************ WIFI ************/
const char* ssid = "Ali Faisal";
const char* password = "unavailable";

/************ PINS ************/
#define TRIG_PIN    5
#define ECHO_PIN    18
#define SERVO_PIN   4

#define RED_LED     25
#define GREEN_LED   26

/************ OLED ************/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/************ OBJECTS ************/
WebServer server(80);
Servo radarServo;

/************ VARIABLES ************/
bool radarON = false;

int servoAngle = 0;
int direction = 1;
int stepSize = 1;

unsigned long lastMove = 0;
long distanceCM = 0;
String ledStatus = "";

/************ HTML PAGE ************/
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 Radar</title>
<style>
body {
  background:black;
  color:#00ff99;
  font-family:Arial;
  text-align:center;
}

canvas {
  background:black;
  border-radius:50%;
  box-shadow:0 0 25px #00ff99;
}

button {
  padding:10px 20px;
  font-size:18px;
  margin-top:10px;
  background:#00ff99;
  border:none;
  cursor:pointer;
}

input { width:300px; }
.warning { color:red; font-size:20px; }
</style>
</head>

<body>

<h1>ESP32 RADAR SYSTEM</h1>

<canvas id="radar" width="400" height="400"></canvas><br>

<button onclick="toggle()">ON / OFF</button><br><br>

Speed:
<input type="range" min="1" max="5" value="1" onchange="setSpeed(this.value)">

<p id="info"></p>
<p id="warn" class="warning"></p>

<script>
const canvas = document.getElementById("radar");
const ctx = canvas.getContext("2d");
const center = canvas.width / 2;
const maxRange = center - 20;

let angle = 0;
let distance = 0;

function toggle() {
  fetch('/toggle');
}

function setSpeed(v) {
  fetch('/speed?val=' + v);
}

function drawRadar() {
  ctx.clearRect(0,0,canvas.width,canvas.height);

  // Radar circles
  ctx.strokeStyle = "#00ff99";
  ctx.lineWidth = 1;
  for(let i=1;i<=4;i++){
    ctx.beginPath();
    ctx.arc(center, center, (maxRange/4)*i, 0, Math.PI*2);
    ctx.stroke();
  }

  // Cross lines
  ctx.beginPath();
  ctx.moveTo(center,0);
  ctx.lineTo(center,canvas.height);
  ctx.moveTo(0,center);
  ctx.lineTo(canvas.width,center);
  ctx.stroke();

  // Sweep line
  let rad = angle * Math.PI / 180;
  ctx.strokeStyle = "#00ff00";
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(center,center);
  ctx.lineTo(
    center + maxRange * Math.cos(rad),
    center - maxRange * Math.sin(rad)
  );
  ctx.stroke();

  // Object dot
  if(distance > 0 && distance < 400){
    let objRange = (distance / 400) * maxRange;
    let x = center + objRange * Math.cos(rad);
    let y = center - objRange * Math.sin(rad);

    ctx.fillStyle = "red";
    ctx.beginPath();
    ctx.arc(x,y,5,0,Math.PI*2);
    ctx.fill();
  }
}

setInterval(() => {
  fetch('/data')
  .then(res => res.json())
  .then(d => {
    angle = d.angle;
    distance = d.dist;

    document.getElementById("info").innerHTML =
      "Angle: " + angle + "° | Distance: " + distance + " cm | LED: " + d.led;

    document.getElementById("warn").innerHTML =
      (distance > 0 && distance < 20) ? "⚠ OBJECT VERY CLOSE!" : "";

    drawRadar();
  });
}, 60);
</script>

</body>
</html>
)rawliteral";


/************ ULTRASONIC ************/
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

/************ LED LOGIC ************/
void updateLEDs(long dist) {
  if (dist > 0 && dist < 40) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    ledStatus = "RED";
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    ledStatus = "GREEN";
  }
}

/************ OLED ************/
void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  display.print("Radar: ");
  display.println(radarON ? "ON" : "OFF");

  display.setCursor(0,14);
  display.print("Angle: ");
  display.print(servoAngle);
  display.println(" deg");

  display.setCursor(0,28);
  display.print("Dist: ");
  display.print(distanceCM);
  display.println(" cm");

  display.setCursor(0,42);
  display.print("LED: ");
  display.println(ledStatus);

  if (distanceCM > 0 && distanceCM < 20) {
    display.setCursor(0,56);
    display.println("WARNING!");
  }

  display.display();
}

/************ WEB ROUTES ************/
void setupServer() {

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });

  server.on("/toggle", []() {
    radarON = !radarON;
    server.send(200, "text/plain", "OK");
  });

  server.on("/speed", []() {
    stepSize = server.arg("val").toInt();
    server.send(200, "text/plain", "OK");
  });

  server.on("/data", []() {
    String json = "{";
    json += "\"angle\":" + String(servoAngle) + ",";
    json += "\"dist\":" + String(distanceCM) + ",";
    json += "\"led\":\"" + ledStatus + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
}

/************ SETUP ************/
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  radarServo.attach(SERVO_PIN, 500, 2400);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nIP Address: ");
  Serial.println(WiFi.localIP());

  setupServer();
}

/************ LOOP ************/
void loop() {
  server.handleClient();

  if (!radarON) {
    radarServo.write(0);
    updateOLED();
    return;
  }

  if (millis() - lastMove >= 30) {
    lastMove = millis();

    servoAngle += direction * stepSize;
    if (servoAngle >= 180) { servoAngle = 180; direction = -1; }
    if (servoAngle <= 0)   { servoAngle = 0; direction = 1; }

    radarServo.write(servoAngle);
  }

  distanceCM = getDistance();
  updateLEDs(distanceCM);
  updateOLED();
}
