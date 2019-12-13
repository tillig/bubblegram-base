#ifndef Light_h
#define Light_h

#include "Arduino.h"
#include "Color.h"

class Light
{
public:
    Light();
    Color currentColor;
    Color targetColor;
    void Tester();
};

#endif
