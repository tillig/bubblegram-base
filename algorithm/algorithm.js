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
        transition: 0,
        newTarget: 1
    }),
    maxTransitionRgb = 5,
    msPerLoop = 100;

var primaryLightIndex = 0,
    stage = loopStage.transition;

(function () {
    lights[0].currentColor.r = 255;
    lights[0].targetColor.r = 255;
    lights[1].currentColor.r = 255;
    lights[1].currentColor.g = 255;
    lights[1].targetColor.r = 255;
    lights[1].targetColor.g = 255;
    lights[2].currentColor.g = 255;
    lights[2].targetColor.g = 255;
    lights[3].currentColor.b = 255;
    lights[3].targetColor.b = 255;
    updateLights();
    setInterval(mainLoop, msPerLoop);
})();

function mainLoop() {
    switch (stage) {
        case loopStage.transition:
            transition();
            if (transitionComplete()) {
                stage = loopStage.newTarget;
            }
            break;
        case loopStage.newTarget:
            primaryLightIndex = getRandomInt(3);
            lights[primaryLightIndex].targetColor.fromHsl(getRandomInt(360), 100, 50);
            stage = loopStage.transition;
            break;
        default:
            break;
    }
    //rotate();
    //rainbow();
    updateLights();
}

function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max + 1));
}

function transition() {
    for (var i = 0; i < lights.length; i++) {
        if (!lights[i].currentColor.equals(lights[i].targetColor)) {
            lights[i].currentColor.r = transitionColorValue(lights[i].currentColor.r, lights[i].targetColor.r);
            lights[i].currentColor.g = transitionColorValue(lights[i].currentColor.g, lights[i].targetColor.g);
            lights[i].currentColor.b = transitionColorValue(lights[i].currentColor.b, lights[i].targetColor.b);
        }
    }
}

function transitionColorValue(current, target) {
    if (current === target) {
        return current;
    }
    var step = Math.min(Math.abs(current - target), maxTransitionRgb);
    return target > current ? current + step : current - step;
}

function transitionComplete() {
    for (var i = 0; i < lights.length; i++) {
        if (!lights[i].currentColor.equals(lights[i].targetColor)) {
            return false;
        }
    }
    return true;
}

function updateLights() {
    for (var index = 0; index < lights.length; index++) {
        if (!lights[index].currentColor.equals(lights[index].targetColor)) {
            document.getElementById(lights[index].id).style.backgroundColor = lights[index].currentColor.toRgbString();
        }
    }
}

function rotate() {
    var previousColor = lights[lights.length - 1].currentColor.clone();
    for (var index = 0; index < lights.length; index++) {
        var nextIndex = index + 1;
        var nextColor = lights[index].currentColor.clone();
        lights[index].currentColor.fromColor(previousColor);
        previousColor = nextColor;
    }

    lights[0].currentColor.fromColor(previousColor);
}

function rainbow() {
    var currentColor = lights[0].currentColor.toHsl(),
        newH = currentColor[0] + 5;
    lights[0].currentColor.fromHsl(newH, currentColor[1], currentColor[2]);
}