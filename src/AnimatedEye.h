/*
 * AnimatedroboEye Library
 * Created by Shourov Paul, 2026.
 * https://github.com/Shourov-Paul/AnimatedroboEye
 *
 * This library is licensed under the GPL-3.0 License.
 *
 * Provides an easy-to-use AnimatedEye class for drawing and animating
 * expressive robot eyes on ST7735 TFT displays.
 */

#ifndef ANIMATED_EYE_H
#define ANIMATED_EYE_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Arduino.h>

class AnimatedEye {
public:
  // Constructor requires only the display pointer
  AnimatedEye(Adafruit_ST7735 *tft);

  // Initialize display settings, geometry, and draw initial eyes
  void begin(int screenWidth, int screenHeight, int maxFps);

  // Update configuration dynamically (width, height, radius, eyecolor, bgcolor
  // optional)
  void setConfiguration(int eyeW, int eyeH, int radius, uint16_t eyeColor,
                        uint16_t bgColor = 0x0000);

  // Animation routines
  void blink();
  void happyShake();
  void happyPop();
  void angry();
  void sad();

private:
  Adafruit_ST7735 *_tft;
  int _cx;
  int _cy;
  int _eyeW;
  int _eyeH;
  int _gap;
  int _radius;
  uint16_t _eyeColor;
  uint16_t _bgColor;
  int _frameDelayMs;

  // Drawing helpers
  void drawEyes(int offsetX, int offsetY, int w, int h, uint16_t color);
  void drawStar(int x, int y, int radius, uint16_t color);
  void redraw();
};

#endif // ANIMATED_EYE_H
