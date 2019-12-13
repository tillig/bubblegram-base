#include <Adafruit_NeoPixel.h>
#include "Color.h"

// Pin connected to WS2812 data.
#define PIN 0

// Number of LEDs to drive.
#define NUMPIXELS 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  strip.begin();
}

void loop()
{
  chase(strip.Color(255, 0, 0)); // Red
  chase(strip.Color(0, 255, 0)); // Green
  chase(strip.Color(0, 0, 255)); // Blue
}

static void chase(uint32_t c)
{
  for (uint16_t i = 0; i < strip.numPixels() + 4; i++)
  {
    strip.setPixelColor(i, c);     // Draw new pixel
    strip.setPixelColor(i - 1, 0); // Erase pixel a few steps back
    strip.show();
    delay(1000);
  }
}
