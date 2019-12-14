#include "LoopStateMachine.h"

LoopStateMachine::LoopStateMachine()
{
    state = STATE_SETNEWPRIMARY;
    primaryLightIndex = 0;
    secondaryLightIndex = 0;
}

uint8_t LoopStateMachine::getState()
{
    return state;
}

void LoopStateMachine::transition()
{
    switch (state)
    {
    case STATE_SETNEWPRIMARY:
        state = STATE_TRANSITIONTONEWPRIMARY;
        break;
    case STATE_TRANSITIONTONEWPRIMARY:
        state = STATE_WAVEINIT;
        break;
    case STATE_WAVEINIT:
        state = STATE_WAVEUP;
        break;
    case STATE_WAVEUP:
        state = STATE_WAVEDOWN;
        break;
    case STATE_WAVEDOWN:
        state = STATE_SETNEWPRIMARY;
        break;
    default:
        break;
    }
}