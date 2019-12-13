/** A particular color that can be expressed in RGB or HSL. */
class Color {
    /**
    * Initializes a new instance of the class.
    */
    constructor() {
        /**
         * Red value.
         * @type {!number}
         */
        this.r = 0;
        /**
         * Green value.
         * @type {!number}
         */
        this.g = 0;
        /**
         * Blue value.
         * @type {!number}
         */
        this.b = 0;
    }

    /**
    * Updates the current color instance from an existing color.
    * @param {!Color} color The color that should be copied.
    */
    fromColor(color) {
        this.r = color.r;
        this.g = color.g;
        this.b = color.b;
    }

    /**
    * Updates the current color instance from an existing color
    * expressed in HSL.
    * @param {!number} h The hue of the color that should be copied (0 - 360).
    * @param {!number} s The saturation of the color that should be copied (0 -
    * 100).
    * @param {!number} l The light of the color that should be copied (0 - 100).
    */
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

    /**
    * Converts the current color to HSL and returns the HSL components in an
    * array - H (0 - 360), S (0 - 100), L (0 - 100).
    * @return {!Array<number>} The current color as HSL.
    */
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

    /**
    * Converts the current color to a CSS-supported RGB string value. Not needed
    * in the Arduino LED version.
    * @returns {!string} The 'rgb(r, g, b)' string version of the color.
    */
    toRgbString() {
        return "rgb(" + this.r + ", " + this.g + ", " + this.b + ")";
    }

    /**
    * Compares one color to this color and determines if they're equal.
    * @param {!Color} color The color to which to compare this color.
    * @returns {!boolean} True if the colors are equal; false if not.
    */
    equals(color) {
        return color !== null && color.r === this.r && color.g === this.g && color.b === this.b;
    }
}

/** State for a single LED. */
class Light {
    /**
    * Initializes a new instance of the class.
    * @param {!string} id The CSS element ID of the light.
    */
    constructor(id) {
        /**
         * The CSS element ID of the light.
         * @type {!string}
         */
        this.id = id;
        /**
         * The current color of the light. Setting this will affect the color
         * when render is called.
         * @type {!Color}
         */
        this.currentColor = new Color();
        /**
         * The target color of the light. Used when transitioning the light from
         * one color to another, this metadata helps track the destination.
         * @type {!Color}
         */
        this.targetColor = new Color();
    }
}

/**
 * States in the loop state machine.
 * @enum {!number}
 */
const LoopStates = Object.freeze({
    /** The loop should choose a new primary light and color. */
    SetNewPrimary: 0,
    /** A new primary light and color has been chosen, currently transitioning
     * to those new values. */
    TransitionToNewPrimary: 1,
    /** The lights are all at the new primary values, execute the 'wave' where
     * the secondary light oscillates. */
    WaveInit: 2,
    /** The lights are in the wave and the color transition for secondary should
     * be going up. */
    WaveUp: 3,
    /** The lights are in the wave and the color transition for secondary should
     * be going down. */
    WaveDown: 4
});

/** Maintains the state for the render loop state machine, handles state
 * transitions. */
class LoopState {
    /**
    * Initializes a new instance of the class.
    */
    constructor() {
        /**
         * The current state of the loop.
         * @type {!LoopStates}
         */
        this.state = LoopStates.SetNewPrimary;

        /** The index of the primary light.
         * @type {!number}
        */
        this.primaryLightIndex = 0;

        /** The index of the secondary light. This should generally be
         * diagonal / across from the primary light.
         * @type {!number}
        */
        this.secondaryLightIndex = 0;
    }

    /** Transitions from the current state to the next state in the machine. */
    transition() {
        switch (this.state) {
            case LoopStates.SetNewPrimary:
                this.state = LoopStates.TransitionToNewPrimary;
                break;
            case LoopStates.TransitionToNewPrimary:
                this.state = LoopStates.WaveInit;
                break;
            case LoopStates.WaveInit:
                this.state = LoopStates.WaveUp;
                break;
            case LoopStates.WaveUp:
                this.state = LoopStates.WaveDown;
                break;
            case LoopStates.WaveDown:
                this.state = LoopStates.SetNewPrimary;
                break;
            default:
                break;
        }
    }
}

/** The set of lights in an addressable array.
 * @type {!Array<Light>}
 */
const lights = [
    new Light("one"),
    new Light("two"),
    new Light("three"),
    new Light("four")
];

/** The object that tracks the loop state.
 * @type {!LoopState}
 */
const stateMonitor = new LoopState();

/** The greatest allowable change on an RGB value that can be made in one loop
 * iteration. Even if the target and the current color differ by 100 times this
 * number, this is the fastest allowable transition.
 * @type {!number}
 */
const maxTransitionRgb = 5;

/** The number of milliseconds to sleep before executing another loop.
 * @type {!number}
 */
const msPerLoop = 100;

(function () {
    setInterval(mainLoop, msPerLoop);
})();

/** The main callback loop. This is analogous to the main loop in an Arduino
 * program, except the callback here is coming from a setInteral. */
function mainLoop() {
    switch (stateMonitor.state) {
        case LoopStates.SetNewPrimary:
            setNewPrimary();
            stateMonitor.transition();
            break;
        case LoopStates.TransitionToNewPrimary:
            smoothTransition();
            if (allLightsAtTarget()) {
                stateMonitor.transition();
            }
            break;
        case LoopStates.WaveInit:
            waveInit();
            stateMonitor.transition();
            break;
        case LoopStates.WaveUp:
            waveUp();
            break;
        case LoopStates.WaveDown:
            waveDown();
            break;
        default:
            break;
    }

    render();
}

/** Handles the "wave init" state, selecting a target for the upward wave colors
 * and transitioning to the next machine state. */
function waveInit() {
    // Wave just beginning, set it to go up.
    var hsl = lights[stateMonitor.primaryLightIndex].currentColor.toHsl(),
        targetH = hsl[0] + 90;
    if (targetH > 360) {
        targetH = targetH - 360;
    }
    lights[stateMonitor.secondaryLightIndex].targetColor.fromHsl(targetH, 100, 50);
    console.log("Wave going UP.");
    console.log("   Primary H = " + hsl[0]);
    console.log("   Target H = " + targetH);
    console.log("   Light " + stateMonitor.secondaryLightIndex + " going to (" + lights[stateMonitor.secondaryLightIndex].targetColor.r + ", " + lights[stateMonitor.secondaryLightIndex].targetColor.g + ", " + lights[stateMonitor.secondaryLightIndex].targetColor.b + ")");
}

/** Handles the "wave up" state, executing a wave transition if the lights
 * aren't all at their target or calculating a wave down target and
 * transitioning to the new machine state if they are. */
function waveUp() {
    // Wave going up.
    if (allLightsAtTarget()) {
        // Wave hit the top, head down.
        var hsl = lights[stateMonitor.primaryLightIndex].currentColor.toHsl(),
            targetH = hsl[0] - 90;
        if (targetH < 0) {
            targetH = 360 - targetH;
        }
        lights[stateMonitor.secondaryLightIndex].targetColor.fromHsl(targetH, 100, 50);
        console.log("Wave going DOWN.");
        console.log("   Primary H = " + hsl[0]);
        console.log("   Target H = " + targetH);
        console.log("   Light " + stateMonitor.secondaryLightIndex + " going to (" + lights[stateMonitor.secondaryLightIndex].targetColor.r + ", " + lights[stateMonitor.secondaryLightIndex].targetColor.g + ", " + lights[stateMonitor.secondaryLightIndex].targetColor.b + ")");
        stateMonitor.transition();
    }

    waveTransition();
}

/** Handles the "wave down" state, executing a wave transition if the lights
 * aren't all at their target or transitioning to the new machine state if they
 * are. */
function waveDown() {
    // Wave going down.
    if (allLightsAtTarget()) {
        // Wave hit the bottom, time to move on.
        stateMonitor.transition();
        console.log("Wave complete. Pick new target.");
        return;
    }

    waveTransition();
}

/** Chooses a new primary light, secondary light, and general color. */
function setNewPrimary() {
    stateMonitor.primaryLightIndex = getRandomInt(lights.length - 1);
    stateMonitor.secondaryLightIndex = (stateMonitor.primaryLightIndex + lights.length / 2) % lights.length;
    var newHue = getRandomInt(360);

    console.log("New standard H: " + newHue);

    for (var i = 0; i < lights.length; i++) {
        lights[i].targetColor.fromHsl(newHue, 100, 50);
    }
}

/** Gets a random integer between zero and a specified maximum.
 * @param {!number} max The largest allowed value to return.
 * @returns {!number} The randomly chosen value.
 */
function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max + 1));
}

/** Executes a "throttled" transition for a single light. Updates the current
 * color on the light to be the transition value, ready for a render loop
 * update.
 * @param {!Light} light The light to transition between current and target
 * colors.
 */
function transitionSingleLight(light) {
    if (!light.currentColor.equals(light.targetColor)) {
        light.currentColor.r = transitionColorValue(light.currentColor.r, light.targetColor.r);
        light.currentColor.g = transitionColorValue(light.currentColor.g, light.targetColor.g);
        light.currentColor.b = transitionColorValue(light.currentColor.b, light.targetColor.b);
    }
}

/** Executes a wave transition in the lights, where the primary light is held
 * stable, the secondary light is making a transition to a specific destination,
 * and all the other lights should be halfway between the primary and secondary.
 */
function waveTransition() {
    transitionSingleLight(lights[stateMonitor.secondaryLightIndex]);
    var targetColor = averageColor(lights[stateMonitor.primaryLightIndex].currentColor, lights[stateMonitor.secondaryLightIndex].currentColor);

    for (var i = 0; i < lights.length; i++) {
        switch (i) {
            case stateMonitor.secondaryLightIndex:
            case stateMonitor.primaryLightIndex:
                break;
            default:
                // Other lights should be halfway between primary and secondary.
                lights[i].currentColor.fromColor(targetColor);
                lights[i].targetColor.fromColor(targetColor);
                break;
        }
    }
}

/** Given two colors, determines a color halfway between based on RGB values.
 * @param {!Color} colorA The first color in the average.
 * @param {!Color} colorB The second color in the average.
 * @returns {!Color} A color halfway between the two passed in.
 */
function averageColor(colorA, colorB) {
    var average = new Color();
    average.r = Math.round((colorA.r + colorB.r) / 2);
    average.g = Math.round((colorA.g + colorB.g) / 2);
    average.b = Math.round((colorA.b + colorB.b) / 2);
    return average;
}

/** Executes a smooth transition for all lights toward each light's target.
 * Accounts for the maximum allowed transition configured.
 */
function smoothTransition() {
    for (var i = 0; i < lights.length; i++) {
        transitionSingleLight(lights[i]);
    }
}

/** Determines if all the lights have their current and target colors identical.
 * @returns {!boolean} True if the lights are all at target values, false if
 * more transition should occur.
 */
function allLightsAtTarget() {
    for (var i = 0; i < lights.length; i++) {
        if (!lights[i].currentColor.equals(lights[i].targetColor)) {
            return false;
        }
    }
    return true;
}

/** Determines the individual color (R, G, or B) that should be transitioned to
 * based on the desired destination and the max amount allowed to transition in
 * a single loop.
 * @param {!number} current The current R, G, or B value of the light.
 * @param {!number} target The desired R, G, or B value of the light.
 * @returns {!number} The R, G, or B value (as passed in) to which the light
 * should be updated on the path to the target.
 */
function transitionColorValue(current, target) {
    if (current === target) {
        return current;
    }
    var step = Math.min(Math.abs(current - target), maxTransitionRgb);
    return target > current ? current + step : current - step;
}

/** Updates the lights with the value in 'current color'. */
function render() {
    // This is where we'll call the method to update the actual LEDs to the
    // appropriate color.
    for (var index = 0; index < lights.length; index++) {
        document.getElementById(lights[index].id).style.backgroundColor = lights[index].currentColor.toRgbString();
    }
    document.getElementById('primaryIndex').innerHTML = "Primary: " + stateMonitor.primaryLightIndex;
    document.getElementById('secondaryIndex').innerHTML = "Secondary: " + stateMonitor.secondaryLightIndex;
}
