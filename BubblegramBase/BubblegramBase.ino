// Target - 6012 bytes
// The full wave up/down is too many instructions. Also, HSL calculations never
// seem to yield bright enough colors with the lights. Instead, let's always
// make the random color based on full RGB values - 255 or 0, and the secondary
// can be between 128 and 255 for a value, but no less. Also, no monochrome.
#include <Adafruit_NeoPixel.h>
#include "Color.h"

// Pin connected to WS2812 data.
#define PIN 0

// Number of LEDs to drive.
#define NUMPIXELS 4

// Amount of time between LED updates.
#define MS_PER_LOOP 200

// The most any RGB value can move in a single loop.
#define MAX_TRANSITION_RGB 5

// The default amount of HSL lightness to use when picking a target color.
#define TARGET_LIGHTNESS_PERCENT 25

// Default brightness of the strip. Darker seems to produce richer colors up to a point.
#define INITIAL_STRIP_BRIGHTNESS 50

// WS2812 lights being controlled.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// The set of current ("to display") colors for the lights.
Color currentColor[NUMPIXELS];

// The set of target ("heading toward") colors for the lights.
Color targetColor[NUMPIXELS];

// The "primary light" - fixed color, other lights move.
uint8_t primaryLightIndex = 0;

// The "secondary light" - leads the wave.
uint8_t secondaryLightIndex = 0;

void setup()
{
  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    currentColor[i] = Color();
    targetColor[i] = Color();
  }

  strip.begin();
  strip.setBrightness(INITIAL_STRIP_BRIGHTNESS);
}

void loop()
{
  primaryLightIndex = rand() % NUMPIXELS;
  secondaryLightIndex = (primaryLightIndex + NUMPIXELS / 2) % NUMPIXELS;
  uint16_t newHue = rand() % 360;
  uint8_t loopIndex = 0;

  for (loopIndex = 0; loopIndex < NUMPIXELS; loopIndex++)
  {
    targetColor[loopIndex].red = 255;
    targetColor[loopIndex].green = 0;
    targetColor[loopIndex].blue = 0;
    //.fromHsl(0, 100, TARGET_LIGHTNESS_PERCENT);
  }

  transitionLoop();

  // Instead of wave up / wave down, we'll just pick one direction and a random
  // amount.
  uint8_t hueDifferential = rand() % 270;
  targetColor[secondaryLightIndex].fromHsl(currentColor[primaryLightIndex].hue + hueDifferential, 100, TARGET_LIGHTNESS_PERCENT);
  for (loopIndex = 0; loopIndex < NUMPIXELS; loopIndex++)
  {
    if (loopIndex != primaryLightIndex && loopIndex != secondaryLightIndex)
    {
      targetColor[loopIndex].red = (targetColor[primaryLightIndex].red + targetColor[secondaryLightIndex].red) / 2;
      targetColor[loopIndex].green = (targetColor[primaryLightIndex].green + targetColor[secondaryLightIndex].green) / 2;
      targetColor[loopIndex].blue = (targetColor[primaryLightIndex].blue + targetColor[secondaryLightIndex].blue) / 2;
    }
  }

  transitionLoop();
}

static void transitionLoop()
{
  while (!allLightsAtTarget())
  {
    for (uint8_t loopIndex = 0; loopIndex < NUMPIXELS; loopIndex++)
    {
      transitionSingleLight(loopIndex);
    }

    show();
    delay(MS_PER_LOOP);
  }
}

void show()
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, currentColor[i].red, currentColor[i].green, currentColor[i].blue);
  }

  strip.show();
}

static bool allLightsAtTarget()
{
  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    if (!currentColor[i].equals(targetColor[i]))
    {
      return false;
    }
  }

  return true;
}

static void transitionSingleLight(uint8_t index)
{
  if (!currentColor[index].equals(targetColor[index]))
  {
    currentColor[index].red = transitionColorValue(currentColor[index].red, targetColor[index].red);
    currentColor[index].green = transitionColorValue(currentColor[index].green, targetColor[index].green);
    currentColor[index].blue = transitionColorValue(currentColor[index].blue, targetColor[index].blue);
  }
}

uint8_t transitionColorValue(uint8_t current, uint8_t target)
{
  if (current == target)
  {
    return current;
  }

  uint8_t step = min(abs(current - target), MAX_TRANSITION_RGB);
  return target > current ? current + step : current - step;
}
