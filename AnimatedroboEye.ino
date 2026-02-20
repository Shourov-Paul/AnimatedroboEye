#include "src/AnimatedEye.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// User defined pins
#define TFT_CS 15
#define TFT_RST 4
#define TFT_DC 2

// Initialize TFT library
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Create AnimatedEye object instance directly.
// Constructor now only takes the TFT pointer.
AnimatedEye robotEyes(&tft);

void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1); // Landscape mode

  // Initialize the driver with screen width, height, and target max_fps
  // This calculates centers dynamically and prepares the display.
  // begin(width, height, max_fps)
  robotEyes.begin(160, 128, 50);

  // You can still dynamically configure the eye shapes if needed!
  // parameters: (width, height, corner_radius, color)
  robotEyes.setConfiguration(40, 40, 10, ST7735_WHITE);
}

void loop() {
  delay(1500);

  // Randomly act
  int r = random(0, 100);
  if (r < 20) {
    robotEyes.blink();
  } else if (r < 40) {
    robotEyes.happyShake();
  } else if (r < 60) {
    robotEyes.happyPop();
  } else if (r < 80) {
    robotEyes.angry();
  } else {
    robotEyes.sad();
  }
}
