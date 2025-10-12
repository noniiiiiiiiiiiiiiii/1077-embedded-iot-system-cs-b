#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED width in pixels
#define SCREEN_HEIGHT 64  // OLED height in pixels
#define OLED_RESET -1     // Reset pin (not used on Wokwi)
#define SCREEN_ADDRESS 0x3C  // I2C address of OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 connection failed"));
    for (;;);
  }

  display.clearDisplay();  // Clear screen
  display.setTextSize(1);  // Text size (1 = normal)
  display.setTextColor(SSD1306_WHITE); // Text color (white pixels)
  display.setCursor(10, 20);  // X=10, Y=20 position of text
  
 
  display.println("umar");
  display.println("1077");
  
  display.drawRect(5, 15, 118, 30, SSD1306_WHITE);

  display.display();  // Show everything on screen
}

void loop() {

}
