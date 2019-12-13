#ifndef LoopStateMachine_h
#define LoopStateMachine_h

#include "Arduino.h"
#include "LoopState.h"

class LoopStateMachine
{
public:
    LoopStateMachine();
    LoopState getState();
    uint8_t primaryLightIndex;
    uint8_t secondaryLightIndex;
    void transition();

private:
    LoopState state;
};

#endif