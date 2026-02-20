# AnimatedroboEye

A lightweight, expressive Arduino library to draw animated robot eyes on ST7735 TFT displays. 

Created by [Shourov Paul](https://github.com/Shourov-Paul).

## Features

- **Customizable:** Easily configure eye width, height, corner radius, gap, and colors dynamically.
- **Flicker-Free:** Employs optimized partial-screen erasing algorithms for buttery-smooth animations.
- **5 Expressive Animations:**
  - `blink()`: A standard, rapid eyelid blink.
  - `happyShake()`: A smooth sine-wave oscillation moving the eyes from side to side.
  - `happyPop()`: A multi-phase animation where the eyes squish happily and explode into an outward radial burst of animated pixel flares/stars.
  - `angry()`: Aggressive, downward-slanted V-shaped brows that violently bulge and pulse outwards.
  - `sad()`: Drooping puppy-dog outer corners paired with a large, slowly falling tear that splashes at the bottom.

## Installation

1. Download this repository as a ZIP file.
2. Open the Arduino IDE.
3. Navigate to **Sketch -> Include Library -> Add .ZIP Library...**
4. Select the downloaded ZIP file.

## Dependencies

This library requires the following Adafruit libraries to handle low-level graphics and hardware communication:
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit ST7735 and ST7789 Library](https://github.com/adafruit/Adafruit-ST7735-Library)

## Usage

You can find a complete, ready-to-flash example inside the Arduino IDE under:
**File -> Examples -> AnimatedroboEye -> AnimatedroboEye**

### Basic Setup Example

```cpp
#include <AnimatedEye.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// User defined pins for the ST7735 Display (ESP32 Example)
#define TFT_CS 15
#define TFT_RST 4
#define TFT_DC 2

// Initialize TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Instantiate AnimatedEye, passing the pointer to your TFT driver
AnimatedEye robotEyes(&tft);

void setup() {
  tft.initR(INITR_BLACKTAB); // Adjust INITR type based on your ST7735 hardware
  tft.setRotation(1);        // Landscape mode

  // Initialize eyes: begin(screenWidth, screenHeight, maxFps)
  robotEyes.begin(160, 128, 50); 
}

void loop() {
  // Play an animation
  robotEyes.blink();
  delay(1500);
}
```

## API Reference

### `AnimatedEye(Adafruit_ST7735 *tft)`
Constructor. Requires a pointer to an initialized `Adafruit_ST7735` object.

### `void begin(int screenWidth, int screenHeight, int maxFps)`
Initializes the coordinates based on screen dimensions and draws the initial eyes to the screen. 
- `screenWidth` / `screenHeight`: Total dimensions of your TFT in pixels.
- `maxFps`: Frames per second rate limiting for animations (e.g. `50`). The library calculates a uniform delay between animation frames based on this.

### `void setConfiguration(int eyeW, int eyeH, int radius, uint16_t eyeColor, uint16_t bgColor = 0x0000)`
Updates the appearance of the eyes at runtime.

### Animation Methods
Each function orchestrates a fully blocking animation routine. When the animation concludes, the eyes smoothly return to their default state.
- `void blink()`
- `void happyShake()`
- `void happyPop()`
- `void angry()`
- `void sad()`

## License

This project is licensed under the GPL-3.0 License - see the `LICENSE` file for details.
