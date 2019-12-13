#ifndef Color_h
#define Color_h

#include "Arduino.h"
#include <math.h>

class Color
{
  public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    Color();
    void fromColor(const Color& color);
    void fromHsl(uint16_t h, float s, float l);
    //int * toHsl();
    //bool equals(Color color);
};

#endif
