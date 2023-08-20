#pragma once

#include <stdint.h>

class NeoPixels
{
public:
    static NeoPixels *createInstance();

    virtual void begin(unsigned int pin)=0;
    virtual void send(uint8_t *buffer, int length)=0;
    virtual bool ready()=0;

    virtual ~NeoPixels() = default;

protected: 
    NeoPixels(){}
};