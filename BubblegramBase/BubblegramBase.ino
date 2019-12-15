// Target - 6012 bytes
// Simple chase demo - 2458 bytes
// Simple blink using color + light classes - 4440 bytes. Need to slim down the color object.
#include <Adafruit_NeoPixel.h>
#include "Light.h"
#include "Color.h"

// Pin connected to WS2812 data.
#define PIN 0

// Number of LEDs to drive.
#define NUMPIXELS 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// The set of lights in an addressable array.
Light lights[NUMPIXELS];

void setup()
{
  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    lights[0] = Light();
  }

  strip.begin();
}

void loop()
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    lights[i].currentColor.fromHsl(100, 100, 50);
  }

  show();

  delay(1000);

  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    lights[i].currentColor.fromHsl(200, 100, 50);
  }

  show();

  delay(1000);
}

void show()
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, lights[i].currentColor.red, lights[i].currentColor.green, lights[i].currentColor.blue);
  }

  strip.show();
}
