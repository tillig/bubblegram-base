#include "Color.h"

Color::Color() {
  red = 0;
  blue = 0;
  green = 0;
}

void Color::fromColor(const Color& color) {
  red = color.red;
  green = color.green;
  blue = color.blue;
}

void Color::fromHsl(uint16_t h, float s, float l)
{
  if (h >= 360){
    h %= 360;
  }

  s = s / 100.0;
  l = l / 100.0;

  float c = (1 - abs(2 * l - 1)) * s;
  float x = c * (1 - abs(h / 60 % 2 - 1));
  float m = l - c / 2;
  float r = 0;
  float g = 0;
  float b = 0;

  if (0 <= h && h < 60)
  {
    r = c;
    g = x;
    b = 0;
  }
  else if (60 <= h && h < 120)
  {
    r = x;
    g = c;
    b = 0;
  }
  else if (120 <= h && h < 180)
  {
    r = 0;
    g = c;
    b = x;
  }
  else if (180 <= h && h < 240)
  {
    r = 0;
    g = x;
    b = c;
  }
  else if (240 <= h && h < 300)
  {
    r = x;
    g = 0;
    b = c;
  }
  else if (300 <= h && h < 360)
  {
    r = c;
    g = 0;
    b = x;
  }

  r = round((r + m) * 255);
  g = round((g + m) * 255);
  b = round((b + m) * 255);

  red = +r;
  green = +g;
  blue = +b;
}
