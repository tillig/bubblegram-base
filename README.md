# bubblegram-base

Arduino-controlled WS281x lights used for a laser crystal / vitrography / bubblegram base unit.

---

When I went to Walt Disney World I got a really cool [bubblegram](https://en.wikipedia.org/wiki/Bubblegram) of one of the Pandora woodsprites from _Avatar_. With it came a small base unit that had some LEDs that would light the bubblegram up in a pleasing way.

The LED pattern was very simple - it had four LEDs (blue, red, green, orange) and would just do a simple pattern by turning individual bulbs on. It looked nice, but I thought it could use a better color range to provide a wider array of visuals.

I had some WS2812 lights left over from another project, so I figured a base that had four of these instead of four fixed-color lights could create some pretty cool stuff.

## Algorithm

I made the basic algorithm in JavaScript so I could try it out in a simple JS/HTML view. That HTML page is in the `algorithm` folder. Given four lights arranged in a square:

- Pick one to be the "primary." The light in the opposite corner is "secondary."
- Pick a random bright color for the primary light - in HSL, that means any hue, 100% saturation, 50% light.
- Fade all the lights to the primary color.
- Cycle the secondary color +/- 180 on the HSL scale - this will be a wave-like rainbow around the primary color. The opposite corner "tertiary" lights should stay halfway between the primary light and secondary light on the RGB scale to create a nice "gradient" between them.
- After the rainbow wave has completed on the secondary and returned back to the primary color, pick a new primary and repeat the process.
