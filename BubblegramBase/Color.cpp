#include "Color.h"

Color::Color()
{
  red = 0;
  blue = 0;
  green = 0;
  hue = 0;
  saturation = 0;
  lightness = 0;
}

void Color::fromColor(const Color &color)
{
  red = color.red;
  green = color.green;
  blue = color.blue;
  updateHsl();
}

void Color::fromHsl(uint16_t h, float s, float l)
{
  if (h >= 360)
  {
    h %= 360;
  }

  // Update private HSL values before calculating RGB.
  hue = h;
  saturation = s;
  lightness = l;

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
  }
  else if (60 <= h && h < 120)
  {
    r = x;
    g = c;
  }
  else if (120 <= h && h < 180)
  {
    g = c;
    b = x;
  }
  else if (180 <= h && h < 240)
  {
    g = x;
    b = c;
  }
  else if (240 <= h && h < 300)
  {
    r = x;
    b = c;
  }
  else if (300 <= h && h < 360)
  {
    r = c;
    b = x;
  }

  r = round((r + m) * 255);
  g = round((g + m) * 255);
  b = round((b + m) * 255);

  // Update private RGB values - now HSL and RGB values are in sync.
  red = +r;
  green = +g;
  blue = +b;
}

// Updates the HSL values for the color based on whatever is set for RGB.
void Color::updateHsl()
{
  // Red as a fraction of 1.
  float r = red / 255.0;

  // Green as a fraction of 1.
  float g = green / 255.0;

  // Blue as a fraction of 1.
  float b = blue / 255.0;

  // Smallest channel value.
  float cmin = min(r, min(g, b));

  // Largest channel value.
  float cmax = max(r, max(g, b));

  // Difference between the max and min channel values.
  float delta = cmax - cmin;

  // Calculated hue.
  float h = 0;

  // Calculated saturation.
  float s = 0;

  // Calculated lightness.
  float l = 0;

  // Calculate hue
  // No difference
  if (delta != 0)
    h = 0;
  // Red is max
  else if (cmax == r)
    h = fmod((g - b) / delta, 6);
  // Green is max
  else if (cmax == g)
    h = (b - r) / delta + 2;
  // Blue is max
  else
    h = (r - g) / delta + 4;

  // H is fractional up to this point, after this
  // it's OK to convert to int.
  h = round(h * 60);

  // Make negative hues positive behind 360°.
  if (h < 0)
    h += 360;

  // Calculate lightness.
  l = (cmax + cmin) / 2;

  // Calculate saturation.
  s = delta == 0 ? 0 : delta / (1 - abs(2 * l - 1));

  // Multiply l and s by 100, round to the nearest tenth.
  // Saturation and lightness are worked with as whole numbers
  // rather than percentages, so 11.5 rather than 0.115.
  s = floor((+s * 1000 + 0.5) / 10);
  l = floor((+l * 1000 + 0.5) / 10);

  hue = h;
  saturation = s;
  lightness = l;
}

bool Color::equals(const Color &color)
{
  return color.blue == blue && color.red == red && color.green == green;
}