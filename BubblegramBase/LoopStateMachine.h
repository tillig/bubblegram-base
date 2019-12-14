#ifndef LoopStateMachine_h
#define LoopStateMachine_h

#include "Arduino.h"

// States in the overall state machine.
#define STATE_SETNEWPRIMARY 0
#define STATE_TRANSITIONTONEWPRIMARY 1
#define STATE_WAVEINIT 2
#define STATE_WAVEUP 3
#define STATE_WAVEDOWN 4

class LoopStateMachine
{
public:
    LoopStateMachine();
    uint8_t getState();
    uint8_t primaryLightIndex;
    uint8_t secondaryLightIndex;
    void transition();

private:
    uint8_t state;
};

#endif