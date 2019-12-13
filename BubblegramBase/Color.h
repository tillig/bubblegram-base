#ifndef Color_h
#define Color_h

#include "Arduino.h"
#include <math.h>

class Color
{
public:
  Color();

  uint8_t getRed();
  uint8_t getGreen();
  uint8_t getBlue();
  void setRed(uint8_t value);
  void setGreen(uint8_t value);
  void setBlue(uint8_t value);

  void fromColor(const Color &color);
  void fromHsl(uint16_t h, float s, float l);
  bool equals(const Color &color);

private:
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint16_t hue;
  float saturation;
  float lightness;

  void updateHsl();
};

#endif
