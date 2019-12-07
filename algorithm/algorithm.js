class Color {
    constructor() {
        this.r = 0;
        this.g = 0;
        this.b = 0;
    }

    fromColor(color) {
        this.r = color.r;
        this.g = color.g;
        this.b = color.b;
    }

    fromHsl(h, s, l) {
        if (h >= 360)
            h %= 360;
        s = s / 100.0;
        l = l / 100.0;

        let c = (1 - Math.abs(2 * l - 1)) * s,
            x = c * (1 - Math.abs(h / 60 % 2 - 1)),
            m = l - c / 2,
            r = 0,
            g = 0,
            b = 0;

        if (0 <= h && h < 60) {
            r = c;
            g = x;
            b = 0;
        } else if (60 <= h && h < 120) {
            r = x;
            g = c;
            b = 0;
        } else if (120 <= h && h < 180) {
            r = 0;
            g = c;
            b = x;
        } else if (180 <= h && h < 240) {
            r = 0;
            g = x;
            b = c;
        } else if (240 <= h && h < 300) {
            r = x;
            g = 0;
            b = c;
        } else if (300 <= h && h < 360) {
            r = c;
            g = 0;
            b = x;
        }

        r = Math.round((r + m) * 255);
        g = Math.round((g + m) * 255);
        b = Math.round((b + m) * 255);

        this.r = +r;
        this.g = +g;
        this.b = +b;
    }

    toHsl() {
        // make r, g, and b fractions of 1
        let r = this.r / 255,
            g = this.g / 255,
            b = this.b / 255,

            // find greatest and smallest channel values
            cmin = Math.min(r, g, b),
            cmax = Math.max(r, g, b),
            delta = cmax - cmin,
            h = 0,
            s = 0,
            l = 0;

        // calculate hue
        // no difference
        if (delta === 0)
            h = 0;
        // red is max
        else if (cmax === r)
            h = (g - b) / delta % 6;
        // green is max
        else if (cmax === g)
            h = (b - r) / delta + 2;
        // blue is max
        else
            h = (r - g) / delta + 4;

        h = Math.round(h * 60);

        // make negative hues positive behind 360°
        if (h < 0)
            h += 360;

        // calculate lightness
        l = (cmax + cmin) / 2;

        // calculate saturation
        s = delta === 0 ? 0 : delta / (1 - Math.abs(2 * l - 1));

        // multiply l and s by 100
        s = +(s * 100).toFixed(1);
        l = +(l * 100).toFixed(1);

        return [h, s, l];
    }

    toRgbString() {
        return "rgb(" + this.r + ", " + this.g + ", " + this.b + ")";
    }

    clone() {
        var c = new Color();
        c.r = this.r;
        c.g = this.g;
        c.b = this.b;
        return c;
    }

    equals(color) {
        return color !== null && color.r === this.r && color.g === this.g && color.b === this.b;
    }
}

class Light {
    constructor(id) {
        this.id = id;
        this.currentColor = new Color();
        this.targetColor = new Color();
    }
}

const lights = [
    new Light("one"),
    new Light("two"),
    new Light("three"),
    new Light("four")
],
    loopStage = Object.freeze({
        setNewPrimary: 0,
        transitionToNewPrimary: 1,
        wave: 2
    }),
    maxTransitionRgb = 5,
    msPerLoop = 100;

var primaryLightIndex = 0,
    secondaryLightIndex = 0,
    stage = loopStage.setNewPrimary,
    waveDirection = 0;

(function () {
    setInterval(mainLoop, msPerLoop);
})();

function mainLoop() {
    switch (stage) {
        case loopStage.setNewPrimary:
            setNewPrimary();
            stage = loopStage.transitionToNewPrimary;
            break;
        case loopStage.transitionToNewPrimary:
            smoothTransition();
            if (allLightsAtTarget()) {
                stage = loopStage.wave;
            }
            break;
        case loopStage.wave:
            wave();
            break;
        default:
            break;
    }

    render();
}

function wave() {
    if (waveDirection == 0) {
        // Wave just beginning, set it to go up.
        waveDirection = 1;
        var hsl = lights[primaryLightIndex].currentColor.toHsl(),
            targetH = hsl[0] + 90;
        if (targetH > 360) {
            targetH = targetH - 360;
        }
        lights[secondaryLightIndex].targetColor.fromHsl(targetH, 100, 50);
        console.log("Wave going UP.");
        console.log("   Primary H = " + hsl[0]);
        console.log("   Target H = " + targetH);
        console.log("   Light " + secondaryLightIndex + " going to (" + lights[secondaryLightIndex].targetColor.r + ", " + lights[secondaryLightIndex].targetColor.g + ", " + lights[secondaryLightIndex].targetColor.b + ")");
        waveTransition();
    } else if (waveDirection == 1) {
        // Wave going up.
        if (allLightsAtTarget()) {
            // Wave hit the top, head down.
            waveDirection = 2
            var hsl = lights[primaryLightIndex].currentColor.toHsl(),
                targetH = hsl[0] - 90;
            if (targetH < 0) {
                targetH = 360 - targetH;
            }
            lights[secondaryLightIndex].targetColor.fromHsl(targetH, 100, 50);
            console.log("Wave going DOWN.");
            console.log("   Primary H = " + hsl[0]);
            console.log("   Target H = " + targetH);
            console.log("   Light " + secondaryLightIndex + " going to (" + lights[secondaryLightIndex].targetColor.r + ", " + lights[secondaryLightIndex].targetColor.g + ", " + lights[secondaryLightIndex].targetColor.b + ")");
        }
        waveTransition();
    } else if (waveDirection == 2) {
        // Wave going down.
        if (allLightsAtTarget()) {
            // Wave hit the bottom, time to move on.
            waveDirection = 0;
            stage = loopStage.setNewPrimary;
            console.log("Wave complete. Pick new target.");
        } else {
            waveTransition();
        }
    }
}

function setNewPrimary() {
    primaryLightIndex = getRandomInt(lights.length - 1);
    secondaryLightIndex = (primaryLightIndex + (lights.length / 2)) % lights.length;
    var newHue = getRandomInt(360);

    console.log("New standard H: " + newHue);

    for (var i = 0; i < lights.length; i++) {
        lights[i].targetColor.fromHsl(newHue, 100, 50);
    }
}

function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max + 1));
}

function transitionSingleLight(light) {
    if (!light.currentColor.equals(light.targetColor)) {
        light.currentColor.r = transitionColorValue(light.currentColor.r, light.targetColor.r);
        light.currentColor.g = transitionColorValue(light.currentColor.g, light.targetColor.g);
        light.currentColor.b = transitionColorValue(light.currentColor.b, light.targetColor.b);
    }
}

function waveTransition() {
    transitionSingleLight(lights[secondaryLightIndex]);
    var targetColor = averageColor(lights[primaryLightIndex].currentColor, lights[secondaryLightIndex].currentColor);

    for (var i = 0; i < lights.length; i++) {
        switch (i) {
            case secondaryLightIndex:
            case primaryLightIndex:
                break;
            default:
                // Other lights should be halfway between primary and secondary.
                lights[i].currentColor.fromColor(targetColor);
                lights[i].targetColor.fromColor(targetColor);
                break;
        }
    }
}

function averageColor(colorA, colorB) {
    var average = new Color();
    average.r = Math.round((colorA.r + colorB.r) / 2);
    average.g = Math.round((colorA.g + colorB.g) / 2);
    average.b = Math.round((colorA.b + colorB.b) / 2);
    return average;
}

function smoothTransition() {
    for (var i = 0; i < lights.length; i++) {
        transitionSingleLight(lights[i]);
    }
}

function allLightsAtTarget() {
    for (var i = 0; i < lights.length; i++) {
        if (!lights[i].currentColor.equals(lights[i].targetColor)) {
            return false;
        }
    }
    return true;
}

function transitionColorValue(current, target) {
    if (current === target) {
        return current;
    }
    var step = Math.min(Math.abs(current - target), maxTransitionRgb);
    return target > current ? current + step : current - step;
}

function render() {
    for (var index = 0; index < lights.length; index++) {
        document.getElementById(lights[index].id).style.backgroundColor = lights[index].currentColor.toRgbString();
    }
    document.getElementById('primaryIndex').innerHTML = "Primary: " + primaryLightIndex;
    document.getElementById('secondaryIndex').innerHTML = "Secondary: " + secondaryLightIndex;
}
