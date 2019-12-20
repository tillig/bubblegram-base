#include <Adafruit_NeoPixel.h>
#include "Color.h"

// Pin connected to WS2812 data.
#define PIN 0

// Number of LEDs to drive.
#define NUMPIXELS 4

// Amount of time between LED updates.
#define MS_PER_LOOP 200

// Amount of time to hold between solid color / different color transitions.
#define MS_PER_HOLD 2500

// The most any RGB value can move in a single loop.
#define MAX_TRANSITION_RGB 5

// Default brightness of the strip. Darker seems to produce richer colors up to a point.
#define INITIAL_STRIP_BRIGHTNESS 100

// Chooses a random number 0, 128, 255 - for selecting reasonably primary RGB
// values.
#define rand_rgb() min(255, 128 * (rand() % 3))

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
  Color newColor = randomPrimary();
  uint8_t loopIndex = 0;

  for (loopIndex = 0; loopIndex < NUMPIXELS; loopIndex++)
  {
    targetColor[loopIndex].fromColor(newColor);
  }

  transitionLoop();

  delay(MS_PER_HOLD);

  // Instead of wave up / wave down, we'll just pick one direction and a random
  // amount.
  for (loopIndex = 0; loopIndex < NUMPIXELS; loopIndex++)
  {
    if (loopIndex == primaryLightIndex)
    {
      continue;
    }

    if (loopIndex == secondaryLightIndex)
    {
      while (newColor.equals(targetColor[primaryLightIndex]))
      {
        newColor = randomPrimary();
      }
      targetColor[loopIndex].fromColor(newColor);
      continue;
    }

    targetColor[loopIndex].red = 0;
    targetColor[loopIndex].green = 0;
    targetColor[loopIndex].blue = 0;
  }

  transitionLoop();

  delay(MS_PER_HOLD);
}

static Color randomPrimary()
{
  Color random = Color();
  while (random.isMonochrome())
  {
    random.red = rand_rgb();
    random.green = rand_rgb();
    random.blue = rand_rgb();
  }

  return random;
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
