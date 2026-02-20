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

    // Draw New (maintaining default configuration)
    drawEyes(nextX, 0, _eyeW, _eyeH, _eyeColor);

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
}

// --- STARBURST HELPER ---
void AnimatedEye::drawStar(int x, int y, int radius, uint16_t color) {
  if (radius <= 0)
    return;
  // A "flare" or "sparkle" is usually just a cross + a smaller diagonal cross
  // Cross
  _tft->drawLine(x - radius, y, x + radius, y, color);
  _tft->drawLine(x, y - radius, x, y + radius, color);

  // Diagonal (approximate for pixel grid, smaller than main cross)
  int diag = (radius * 5) / 10; // ~0.5 * radius
  if (diag > 0) {
    _tft->drawLine(x - diag, y - diag, x + diag, y + diag, color);
    _tft->drawLine(x - diag, y + diag, x + diag, y - diag, color);
  }
}

// --- HAPPY POP ANIMATION ---
void AnimatedEye::happyPop() {
  int minDim = 4; // Smallest size before disappearing
  int steps = 15; // More steps to shrink slower and smoother

  // Phase 1: Shrink slowly and squish (happy emotional way)
  for (int i = 1; i <= steps; i++) {
    int curW = _eyeW - (i * (_eyeW - minDim) / steps);
    int curH = _eyeH - (i * (_eyeH - minDim) / steps);

    // Jump slightly upward for the happy emotion
    int jumpY = -(i * 6 / steps);

    // Clear area by redrawing background
    _tft->fillScreen(_bgColor);
    drawEyes(0, jumpY, curW, curH, _eyeColor);
    delay(_frameDelayMs * 2); // Play it a bit slower
  }

  // Erase the tiny dots
  _tft->fillScreen(_bgColor);
  delay(_frameDelayMs * 4); // Anticipation pause

  // Phase 2: Pop (Sprinkle multiple flares/stars)
  int leftEyeCenterX = _cx - (_gap / 2) - (_eyeW / 2);
  int rightEyeCenterX = _cx + (_gap / 2) + (_eyeW / 2);
  int eyeCenterY = _cy - 6; // Match the jumpY elevation

  uint16_t starColor = 0xFFE0;  // Yellow
  uint16_t starColor2 = 0xFFFF; // White

  // Define a local struct to keep track of moving sparkles
  struct Sparkle {
    float x, y;
    float dx, dy;
    int radius;
    uint16_t color;
  };

  const int NUM_SPARKLES = 12; // 6 per eye
  Sparkle sparkles[NUM_SPARKLES];

  for (int i = 0; i < NUM_SPARKLES; i++) {
    bool isLeftEye = (i < NUM_SPARKLES / 2);
    int eyeCenterX = isLeftEye ? leftEyeCenterX : rightEyeCenterX;

    // Start slightly randomized around the eye center
    sparkles[i].x = eyeCenterX + random(-8, 8);
    sparkles[i].y = eyeCenterY + random(-8, 8);

    // Explode outward radially from center
    float angle = random(0, 360) * PI / 180.0;
    float speed = random(15, 35) / 10.0; // 1.5 to 3.5 pixels per frame
    sparkles[i].dx = cos(angle) * speed;
    sparkles[i].dy = sin(angle) * speed;

    sparkles[i].radius = random(3, 8);
    sparkles[i].color = (random(0, 2) == 0) ? starColor : starColor2;
  }

  // Animate the pop for ~1000ms
  int frames = 1000 / _frameDelayMs;
  for (int f = 0; f < frames; f++) {
    // Erase old positions
    for (int i = 0; i < NUM_SPARKLES; i++) {
      if (sparkles[i].radius > 0) {
        drawStar((int)sparkles[i].x, (int)sparkles[i].y, sparkles[i].radius,
                 _bgColor);
      }
    }

    // Move and draw new positions
    for (int i = 0; i < NUM_SPARKLES; i++) {
      sparkles[i].x += sparkles[i].dx;
      sparkles[i].y += sparkles[i].dy;

      // Calculate current radius (optionally shrink them as they fade)
      int currentRadius = sparkles[i].radius;
      if (f > frames * 2 /
                  3) { // Start fading out in the last third of the animation
        currentRadius -= (f - frames * 2 / 3) / 2;
        if (currentRadius < 0)
          currentRadius = 0;
      }

      if (currentRadius > 0) {
        drawStar((int)sparkles[i].x, (int)sparkles[i].y, currentRadius,
                 sparkles[i].color);
      }
    }
    delay(_frameDelayMs);
  }

  // Phase 3: Restore
  _tft->fillScreen(_bgColor);
  delay(_frameDelayMs * 3);
  redraw(); // Resets to original eyes
}
