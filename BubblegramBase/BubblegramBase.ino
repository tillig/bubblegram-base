// Before refactor - 7132 bytes
// After removing LoopStates enum - 7106
#include <Adafruit_NeoPixel.h>
#include "Color.h"
#include "Light.h"
#include "LoopStateMachine.h"

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

// The LED strip being driven.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// The object that tracks the loop state.
LoopStateMachine stateMonitor = LoopStateMachine();

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
  switch (stateMonitor.getState())
  {
  case STATE_SETNEWPRIMARY:
    setNewPrimary();
    stateMonitor.transition();
    break;
  case STATE_TRANSITIONTONEWPRIMARY:
    smoothTransition();
    if (allLightsAtTarget())
    {
      stateMonitor.transition();
    }
    break;
  case STATE_WAVEINIT:
    waveInit();
    stateMonitor.transition();
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

/*
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
*/

// Updates the lights with the value in 'current color'.
void render()
{
  // TODO: This is where we'll call the method to update the actual LEDs to the
  // appropriate color.
  /*
  for (var index = 0; index < lights.length; index++)
  {
    document.getElementById(lights[index].id).style.backgroundColor = lights[index].currentColor.toRgbString();
  }
  document.getElementById('primaryIndex').innerHTML = "Primary: " + stateMonitor.primaryLightIndex;
  document.getElementById('secondaryIndex').innerHTML = "Secondary: " + stateMonitor.secondaryLightIndex;
  */
  strip.show();
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
  stateMonitor.primaryLightIndex = rand() % NUMPIXELS;
  stateMonitor.secondaryLightIndex = (stateMonitor.primaryLightIndex + NUMPIXELS / 2) % NUMPIXELS;
  uint16_t newHue = rand() % 360;

  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    lights[i].targetColor.fromHsl(newHue, 100, 50);
  }
}

/* Executes a smooth transition for all lights toward each light's target.
 * Accounts for the maximum allowed transition configured.
 */
void smoothTransition()
{
  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    transitionSingleLight(lights[i]);
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
    light.currentColor.setRed(transitionColorValue(light.currentColor.getRed(), light.targetColor.getRed()));
    light.currentColor.setGreen(transitionColorValue(light.currentColor.getGreen(), light.targetColor.getGreen()));
    light.currentColor.setBlue(transitionColorValue(light.currentColor.getBlue(), light.targetColor.getBlue()));
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
    stateMonitor.transition();
    return;
  }

  waveTransition();
}

/* Handles the "wave init" state, selecting a target for the upward wave colors
 * and transitioning to the next machine state. */
void waveInit()
{
  // Wave just beginning, set it to go up.
  uint16_t targetH = lights[stateMonitor.primaryLightIndex].currentColor.getHue() + 90;

  if (targetH > 360)
  {
    targetH = targetH - 360;
  }

  lights[stateMonitor.secondaryLightIndex].targetColor.fromHsl(targetH, 100, 50);
}

/* Executes a wave transition in the lights, where the primary light is held
 * stable, the secondary light is making a transition to a specific destination,
 * and all the other lights should be halfway between the primary and secondary.
 */
void waveTransition()
{
  transitionSingleLight(lights[stateMonitor.secondaryLightIndex]);
  Color targetColor = Color();
  targetColor.setRed(round((lights[stateMonitor.primaryLightIndex].currentColor.getRed() + lights[stateMonitor.secondaryLightIndex].currentColor.getRed()) / 2));
  targetColor.setGreen(round((lights[stateMonitor.primaryLightIndex].currentColor.getGreen() + lights[stateMonitor.secondaryLightIndex].currentColor.getGreen()) / 2));
  targetColor.setBlue(round((lights[stateMonitor.primaryLightIndex].currentColor.getBlue() + lights[stateMonitor.secondaryLightIndex].currentColor.getBlue()) / 2));

  for (uint8_t i = 0; i < NUMPIXELS; i++)
  {
    if (i != stateMonitor.secondaryLightIndex && i != stateMonitor.primaryLightIndex)
    {
      // Other lights should be halfway between primary and secondary.
      lights[i].currentColor.fromColor(targetColor);
      lights[i].targetColor.fromColor(targetColor);
    }
  }
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
    uint16_t targetH = lights[stateMonitor.primaryLightIndex].currentColor.getHue() - 90;

    if (targetH < 0)
    {
      targetH = 360 - targetH;
    }

    lights[stateMonitor.secondaryLightIndex].targetColor.fromHsl(targetH, 100, 50);
    stateMonitor.transition();
  }

  waveTransition();
}
