// Target - 6012 bytes
// Before refactor - 7132
// After removing LoopStates enum - 7106
// After removing LoopStateManager - 7072
// Adding in actual rendering logic - 7314
// Removed Color get methods - 7220
// Removed Color set methods - 7130
// Inline smoothTransition - 7126
// Removed nextState method - 7088
// Removed waveInit and extra instructions - 7078
#include <WS2811.h>
#include "Color.h"
#include "Light.h"

// Pin connected to WS2812 data.
#define PIN 0

// Number of LEDs to drive.
#define NUMPIXELS 4

/* The greatest allowable change on an RGB value that can be made in one loop
 * iteration. Even if the target and the current color differ by 100 times this
 * number, this is the fastest allowable transition.
 */
#define MAX_TRANSITION_RGB 5

// The number of milliseconds to sleep before executing another loop.
#define MS_PER_LOOP 100

// State machine: Set a new primary color/light.
#define STATE_SETNEWPRIMARY 0

// State machine: Transition all lights to the new primary color.
#define STATE_TRANSITIONTONEWPRIMARY 1

// State machine: Get ready to do the wave up/down on secondary light.
#define STATE_WAVEINIT 2

// State machine: Take the secondary light up.
#define STATE_WAVEUP 3

// State machine: Take the secondary light down.
#define STATE_WAVEDOWN 4

// Define a function that can control the strip. This is one of the Digispark libraries.
DEFINE_WS2811_FN(WS2811RGB, PORTB, PIN)

// Track the RGB values in the strip.
RGB_t rgb[4];

// The current state of the state machine.
uint8_t state = STATE_SETNEWPRIMARY;

// The primary light in the array of lights.
uint8_t primaryLightIndex = 0;

// The secondary light ("opposite corner") in the array of lights.
uint8_t secondaryLightIndex = 0;

// The set of lights in an addressable array.
Light lights[NUMPIXELS];

void setup()
{
  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    lights[0] = Light();
  }

  pinMode(PIN, OUTPUT);
}

void loop()
{
  switch (state)
  {
  case STATE_SETNEWPRIMARY:
    setNewPrimary();
    state = STATE_TRANSITIONTONEWPRIMARY;
    break;
  case STATE_TRANSITIONTONEWPRIMARY:
    for (uint8_t i = 0; i < NUMPIXELS; i++)
    {
      transitionSingleLight(lights[i]);
    };
    if (allLightsAtTarget())
    {
      state = STATE_WAVEINIT;
    }
    break;
  case STATE_WAVEINIT:
    lights[secondaryLightIndex].targetColor.fromHsl(lights[primaryLightIndex].currentColor.hue + 90, 100, 50);
    state = STATE_WAVEUP;
    break;
  case STATE_WAVEUP:
    waveUp();
    break;
  case STATE_WAVEDOWN:
    waveDown();
    break;
  default:
    break;
  }

  render();
  delay(MS_PER_LOOP);
}

// Updates the lights with the value in 'current color'.
void render()
{
  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    rgb[i].r = lights[i].currentColor.red;
    rgb[i].g = lights[i].currentColor.green;
    rgb[i].b = lights[i].currentColor.blue;
  }

  WS2811RGB(rgb, ARRAYLEN(rgb));
}

// Determines if all the lights have their current and target colors identical.
bool allLightsAtTarget()
{
  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    if (!lights[i].currentColor.equals(lights[i].targetColor))
    {
      return false;
    }
  }
  return true;
}

// Chooses a new primary light, secondary light, and general color.
void setNewPrimary()
{
  primaryLightIndex = rand() % NUMPIXELS;
  secondaryLightIndex = (primaryLightIndex + NUMPIXELS / 2) % NUMPIXELS;
  uint16_t newHue = rand() % 360;

  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    lights[i].targetColor.fromHsl(newHue, 100, 50);
  }
}

/** Determines the individual color (R, G, or B) that should be transitioned to
 * based on the desired destination and the max amount allowed to transition in
 * a single loop.
 */
uint8_t transitionColorValue(uint8_t current, uint8_t target)
{
  if (current == target)
  {
    return current;
  }
  uint8_t step = min(abs(current - target), MAX_TRANSITION_RGB);
  return target > current ? current + step : current - step;
};

/* Executes a "throttled" transition for a single light. Updates the current
 * color on the light to be the transition value, ready for a render loop
 * update.
 */
void transitionSingleLight(Light light)
{
  if (!light.currentColor.equals(light.targetColor))
  {
    light.currentColor.red = transitionColorValue(light.currentColor.red, light.targetColor.red);
    light.currentColor.green = transitionColorValue(light.currentColor.green, light.targetColor.green);
    light.currentColor.blue = transitionColorValue(light.currentColor.blue, light.targetColor.blue);
    light.currentColor.updateHsl();
  }
}


/* Executes a wave transition in the lights, where the primary light is held
 * stable, the secondary light is making a transition to a specific destination,
 * and all the other lights should be halfway between the primary and secondary.
 */
void waveTransition()
{
  transitionSingleLight(lights[secondaryLightIndex]);
  Color targetColor = Color();
  targetColor.red = round((lights[primaryLightIndex].currentColor.red + lights[secondaryLightIndex].currentColor.red) / 2);
  targetColor.green = round((lights[primaryLightIndex].currentColor.green + lights[secondaryLightIndex].currentColor.green) / 2);
  targetColor.blue = round((lights[primaryLightIndex].currentColor.blue + lights[secondaryLightIndex].currentColor.blue) / 2);
  targetColor.updateHsl();

  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    if (i != secondaryLightIndex && i != primaryLightIndex)
    {
      // Other lights should be halfway between primary and secondary.
      lights[i].currentColor.fromColor(targetColor);
      lights[i].targetColor.fromColor(targetColor);
    }
  }
}

/* Handles the "wave down" state, executing a wave transition if the lights
 * aren't all at their target or transitioning to the new machine state if they
 * are. */
void waveDown()
{
  // Wave going down.
  if (allLightsAtTarget())
  {
    // Wave hit the bottom, time to move on.
    state = STATE_SETNEWPRIMARY;
    return;
  }

  waveTransition();
}

/* Handles the "wave up" state, executing a wave transition if the lights
 * aren't all at their target or calculating a wave down target and
 * transitioning to the new machine state if they are. */
void waveUp()
{
  // Wave going up.
  if (allLightsAtTarget())
  {
    // Wave hit the top, head down.
    // + 270 because we actually want -90, but then we can get below
    // 360 into negative numbers. Since fromHsl already handles >= 360
    // we'll just add 360, so it's -90 + 360, or +270.
    uint16_t targetH = lights[primaryLightIndex].currentColor.hue + 270;
    lights[secondaryLightIndex].targetColor.fromHsl(targetH, 100, 50);
    state = STATE_WAVEDOWN;
  }

  waveTransition();
}
