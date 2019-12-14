#ifndef Color_h
#define Color_h

#include "Arduino.h"
#include <math.h>

class Color
{
public:
  Color();

  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint16_t hue;
  float saturation;
  float lightness;

  void fromColor(const Color &color);
  void fromHsl(uint16_t h, float s, float l);
  bool equals(const Color &color);
  void updateHsl();
};

#endif
