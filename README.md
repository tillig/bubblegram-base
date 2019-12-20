# bubblegram-base

Arduino-controlled WS281x lights used for a laser crystal / vitrography / bubblegram base unit.

---

When I went to Walt Disney World I got a really cool [bubblegram](https://en.wikipedia.org/wiki/Bubblegram) of one of the Pandora woodsprites from _Avatar_. With it came a small base unit that had some LEDs that would light the bubblegram up in a pleasing way.

The LED pattern was very simple - it had four LEDs (blue, red, green, orange) arranged in a square and would just do a simple pattern by turning individual bulbs on. It looked nice, but I thought it could use a better color range to provide a wider array of visuals.

I had some WS2812 lights left over from another project, so I figured a base that had four of these instead of four fixed-color lights could create some pretty cool stuff.

## Algorithm

I started with a fairly fancy thing where it would fade from one color to the next and do a lot of mixes... but I found out some stuff about bubblegrams.

- Fewer colors is better. After everything gets diffused in there, lots of colors turn out being indistinguishable.
- Color grouping is better. If you can have two lights next to each other (instead of diagonal opposites) being the same color, that'll be better.
- Primary bright colors are better. Subtle pastels and things don't show up well.

I also found that random is really hard to debug on a small board with no console. Is my algorithm wrong or is it just randomly not choosing this combination?

In the end, I went simple:

- A "primary" light index starts at zero.
- Choose a random color.
- All the lights fade to a randomly chosen color.
- A new random color is chosen.
  - The primary light and the half of the lights adjacent to it stay the original color.
  - The rest of the lights fade to the new color.
- The primary light index increments. Loop!

## Board

I'm using a Digispark ATTiny85 board, so I don't need an Arduino as an in-series programmer (ISP). However, it does require [some extra setup for the Arduino IDE](https://digistump.com/wiki/digispark/tutorials/connecting). [This video is worth watching](https://www.youtube.com/watch?v=MmDBvgrYGZs) because it shows on Windows how to get the drivers properly installed.

I noticed that it's very picky on my machine which USB port the thing is plugged into. On one port the Digispark gets detected; on another it just doesn't register at all. I'm not sure why this is.

I also noticed [it's best to unplug the Digispark, start the sketch upload from Arduino IDE, and _then_ plug it in when asked](https://digistump.com/board/index.php?topic=100.0) or the sketch fails to upload with an error `Assertion failed: res >= 4, file library/micronucleus_lib.c, line 63`. Unclear if this is something that could be fixed by updating the [micronucleus firmware from the default version 1.02 that's installed](https://github.com/micronucleus/micronucleus) but Digispark claims to not support that and it wasn't worth fighting.

## IDE Setup

If you're doing it the same way I did, you'll need to update the Arduino IDE with:

- The Digistump boards installed [as noted on the tutorial](https://digistump.com/wiki/digispark/tutorials/connecting).
- The Adafruit Neopixel library

If you're using the VS Code extension for Arduino, you may see some squiggly red marks and compilation failures if you don't have that stuff. Also, check the `c_cpp_properties.json` for C++ include path updates you may need to do.
