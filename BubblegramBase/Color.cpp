#include "Color.h"

Color::Color()
{
  red = 0;
  blue = 0;
  green = 0;
}

void Color::fromColor(const Color &color)
{
  red = color.red;
  green = color.green;
  blue = color.blue;
}

bool Color::equals(const Color &color)
{
  return color.blue == blue && color.red == red && color.green == green;
}

bool Color::isMonochrome()
{
  return red == green && green == blue;
}