#ifndef LoopState_h
#define LoopState_h

#include "Arduino.h"

enum LoopState
{
    SetNewPrimary,
    TransitionToNewPrimary,
    WaveInit,
    WaveUp,
    WaveDown
};

#endif