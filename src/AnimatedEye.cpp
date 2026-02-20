#include "AnimatedEye.h"

// Constructor
AnimatedEye::AnimatedEye(Adafruit_ST7735 *tft) {
  _tft = tft;
  _cx = 80;
  _cy = 64;
  _eyeW = 45;
  _eyeH = 45;
  _gap = 20;
  _radius = 8;
  _eyeColor = ST7735_WHITE; // Default
  _bgColor = 0x0000;        // Default ST7735_BLACK
  _frameDelayMs = 20;       // Default (50 FPS)
}

// Initialize the display geometry and draw initial eyes
void AnimatedEye::begin(int screenWidth, int screenHeight, int maxFps) {
  _cx = screenWidth / 2;
  _cy = screenHeight / 2;
  if (maxFps > 0) {
    _frameDelayMs = 1000 / maxFps;
  }

  redraw();
}

void AnimatedEye::setConfiguration(int eyeW, int eyeH, int radius,
                                   uint16_t eyeColor, uint16_t bgColor) {
  _eyeW = eyeW;
  _eyeH = eyeH;
  _radius = radius;
  _eyeColor = eyeColor;
  _bgColor = bgColor;
  redraw(); // Redraw with new settings
}

// Draw/Redraw initial eyes and clear screen
void AnimatedEye::redraw() {
  _tft->fillScreen(_bgColor);
  drawEyes(0, 0, _eyeW, _eyeH, _eyeColor);
}

// Global drawing helper - draws filled round rects
void AnimatedEye::drawEyes(int offsetX, int offsetY, int w, int h,
                           uint16_t color) {
  int leftEyeX = _cx - (_gap / 2) - w;
  int rightEyeX = _cx + (_gap / 2);
  int eyeY = _cy - (h / 2);

  // Clamp radius for small heights
  int r = _radius;
  if (h < 2 * r) {
    r = h / 2;
  }

  _tft->fillRoundRect(leftEyeX + offsetX, eyeY + offsetY, w, h, r, color);
  _tft->fillRoundRect(rightEyeX + offsetX, eyeY + offsetY, w, h, r, color);
}

// Helper to draw the "Happy" eyes
// Both Eyes: Flat on Left, Round on Right
void AnimatedEye::drawHappyEyes(int offsetX, int offsetY, int w, int h,
                                uint16_t color) {
  int leftEyeX = _cx - (_gap / 2) - w + offsetX;
  int rightEyeX = _cx + (_gap / 2) + offsetX;
  int eyeY = _cy - (h / 2) + offsetY;

  int r = _radius;
  if (h < 2 * r)
    r = h / 2;

  // LEFT EYE: Flat on Left, Round on Right
  _tft->fillRoundRect(leftEyeX, eyeY, w, h, r, color);
  _tft->fillRect(leftEyeX, eyeY, r, h, color);

  // RIGHT EYE: Flat on Left, Round on Right
  _tft->fillRoundRect(rightEyeX, eyeY, w, h, r, color);
  _tft->fillRect(rightEyeX, eyeY, r, h, color);
}

// --- FLICKER-FREE BLINK ---
void AnimatedEye::blink() {
  int closeStep = 6;
  int openStep = 8;
  int minH = 2; // "Closed" height

  // Close
  for (int h = _eyeH; h > minH; h -= closeStep) {
    int nextH = h - closeStep;
    if (nextH < minH)
      nextH = minH;

    int leftX = _cx - (_gap / 2) - _eyeW;
    int rightX = _cx + (_gap / 2);

    int oldTop = _cy - h / 2;
    int newTop = _cy - nextH / 2;
    int eraseH = newTop - oldTop;

    if (eraseH > 0) {
      // Erase Top Strip
      _tft->fillRect(leftX, oldTop, _eyeW, eraseH, _bgColor);
      _tft->fillRect(rightX, oldTop, _eyeW, eraseH, _bgColor);

      // Erase Bottom Strip
      int oldBotLimit = oldTop + h;
      int newBotLimit = newTop + nextH;

      _tft->fillRect(leftX, newBotLimit, _eyeW, oldBotLimit - newBotLimit,
                     _bgColor);
      _tft->fillRect(rightX, newBotLimit, _eyeW, oldBotLimit - newBotLimit,
                     _bgColor);
    }

    drawEyes(0, 0, _eyeW, nextH, _eyeColor);
    delay(_frameDelayMs);
  }

  // Hold Closed
  // Make sure to hold at least 50ms, or more if frame delay is long
  int holdTime = _frameDelayMs > 50 ? _frameDelayMs : 50;
  delay(holdTime);

  // Open
  for (int h = minH; h < _eyeH; h += openStep) {
    int nextH = h + openStep;
    if (nextH > _eyeH)
      nextH = _eyeH;

    drawEyes(0, 0, _eyeW, nextH, _eyeColor);
    delay(_frameDelayMs);
  }
}

// --- SMOOTH OSCILLATION SHAKE ---
void AnimatedEye::happyShake() {
  int lastX = 0;

  // Oscillation Parameters
  float amplitude = 10.0;
  float frequency = 0.4;
  int duration = 40;

  // Helper to move to a new X position cleanly
  auto moveTo = [&](int nextX) {
    if (nextX == lastX)
      return;

    int dx = nextX - lastX;

    // Calculate Base Coords
    int lxBase = _cx - (_gap / 2) - _eyeW;
    int rxBase = _cx + (_gap / 2);
    int yBase = _cy - (_eyeH / 2);

    // Erase Logic (Clearing Trails)
    if (dx > 0) {
      // Moving Right. Erase Left Edge.
      _tft->fillRect(lxBase + lastX, yBase, dx, _eyeH, _bgColor);
      _tft->fillRect(rxBase + lastX, yBase, dx, _eyeH, _bgColor);
    } else {
      // Moving Left. Erase Right Edge.
      _tft->fillRect(lxBase + nextX + _eyeW, yBase, -dx, _eyeH, _bgColor);
      _tft->fillRect(rxBase + nextX + _eyeW, yBase, -dx, _eyeH, _bgColor);
    }

    // Draw New (using custom shape)
    drawHappyEyes(nextX, 0, _eyeW, _eyeH, _eyeColor);

    lastX = nextX;
    delay(_frameDelayMs);
  };

  // Sine Wave Loop
  for (int i = 0; i <= duration; i++) {
    float angle = (float)i / duration * 6.0 * PI;
    int x = (int)(amplitude * sin(angle));
    moveTo(x);
  }

  // Return to Center
  moveTo(0);

  // Restore Normal Eyes (Round-Round)
  drawHappyEyes(0, 0, _eyeW, _eyeH, _bgColor); // Clear happy shape
  drawEyes(0, 0, _eyeW, _eyeH, _eyeColor);     // Draw normal shape
}
