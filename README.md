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

## Board

I'm using a Digispark ATTiny85 board, so I don't need an Arduino as an in-series programmer (ISP). However, it does require [some extra setup for the Arduino IDE](https://digistump.com/wiki/digispark/tutorials/connecting). [This video is worth watching](https://www.youtube.com/watch?v=MmDBvgrYGZs) because it shows on Windows how to get the drivers properly installed.

I noticed that it's very picky on my machine which USB port the thing is plugged into. On one port the Digispark gets detected; on another it just doesn't register at all. I'm not sure why this is.

I also noticed [it's best to unplug the Digispark, start the sketch upload from Arduino IDE, and _then_ plug it in when asked](https://digistump.com/board/index.php?topic=100.0) or the sketch fails to upload with an error `Assertion failed: res >= 4, file library/micronucleus_lib.c, line 63`. Unclear if this is something that could be fixed by updating the [micronucleus firmware from the default version 1.02 that's installed](https://github.com/micronucleus/micronucleus) but Digispark claims to not support that and it wasn't worth fighting.

## IDE Setup

If you're doing it the same way I did, you'll need to update the Arduino IDE with:

- The Digistump boards installed [as noted on the tutorial](https://digistump.com/wiki/digispark/tutorials/connecting).
- The Adafruit Neopixel library

If you're using the VS Code extension for Arduino, you may see some squiggly red marks and compilation failures if you don't have that stuff. Also, check the `c_cpp_properties.json` for C++ include path updates you may need to do.
