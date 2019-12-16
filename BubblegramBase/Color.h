#ifndef Color_h
#define Color_h

#include "Arduino.h"

class Color
{
public:
  Color();

  uint8_t red;
  uint8_t green;
  uint8_t blue;

  void fromColor(const Color &color);
  bool equals(const Color &color);
  bool isMonochrome();
};

#endif
