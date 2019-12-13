#include "LoopStateMachine.h"

LoopStateMachine::LoopStateMachine()
{
    state = SetNewPrimary;
    primaryLightIndex = 0;
    secondaryLightIndex = 0;
}

LoopState LoopStateMachine::getState()
{
    return state;
}

void LoopStateMachine::transition()
{
    switch (state)
    {
    case SetNewPrimary:
        state = TransitionToNewPrimary;
        break;
    case TransitionToNewPrimary:
        state = WaveInit;
        break;
    case WaveInit:
        state = WaveUp;
        break;
    case WaveUp:
        state = WaveDown;
        break;
    case WaveDown:
        state = SetNewPrimary;
        break;
    default:
        break;
    }
}