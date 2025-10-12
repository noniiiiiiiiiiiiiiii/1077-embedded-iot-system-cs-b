#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;); // agar initialize na ho to yahan ruk ja
  }

  display.clearDisplay();

  // 👀 Emoji Face
  // Face outline
  display.drawCircle(64, 32, 20, SSD1306_WHITE);

  // Eyes
  display.fillCircle(56, 27, 2, SSD1306_WHITE);  // left eye
  display.fillCircle(72, 27, 2, SSD1306_WHITE);  // right eye

  display.drawLine(56, 40, 72, 40, SSD1306_WHITE);
  display.drawPixel(57, 41, SSD1306_WHITE);
  display.drawPixel(58, 42, SSD1306_WHITE);
  display.drawPixel(59, 43, SSD1306_WHITE);
  display.drawPixel(60, 43, SSD1306_WHITE);
  display.drawPixel(61, 43, SSD1306_WHITE);
  display.drawPixel(62, 42, SSD1306_WHITE);
  display.drawPixel(63, 41, SSD1306_WHITE);
  display.drawPixel(64, 40, SSD1306_WHITE);

  // Text label
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(40, 55);
  display.println("My Emoji :)");

  display.display();  // show everything
}

void loop() {
  // kuch nahi karna, static display hai
}
