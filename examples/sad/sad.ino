#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
/*
 * AnimatedroboEye - sad Example
 *
 * Created by Shourov Paul, 2026.
 * https://github.com/Shourov-Paul/AnimatedroboEye
 *
 * This library is licensed under the GPL-3.0 License.
 *
 * This example demonstrates how to initialize the AnimatedroboEye library
 * and execute only the sad() animation in a loop.
 */

#include <AnimatedroboEye.h>
#include <SPI.h>

// User defined pins
#define TFT_CS 15
#define TFT_RST 4
#define TFT_DC 2

// Initialize TFT library
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Create AnimatedroboEye object instance directly.
// Constructor now only takes the TFT pointer.
AnimatedroboEye robotEyes(&tft);

void setup() {
  Serial.begin(115200);

  // Initialize display
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1); // Landscape mode

  // Initialize the driver with screen width, height, and target max_fps
  // This calculates centers dynamically and prepares the display.
  // begin(width, height, max_fps)
  robotEyes.begin(160, 128, 50);

  // Dynamically configure the eye shapes
  // parameters: (width, height, corner_radius, color)
  robotEyes.setConfiguration(40, 40, 10, ST7735_WHITE);
}

void loop() {
  delay(2000); // Wait 2 seconds

  // Execute the sad animation
  robotEyes.sad();
}
