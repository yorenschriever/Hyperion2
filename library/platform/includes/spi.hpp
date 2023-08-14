#pragma once

#include <stdint.h>

class SPI
{
public:
    virtual void begin(uint8_t clkPin, uint8_t dataPin, int frq) =0;
    virtual void send(uint8_t *buffer, int length)=0;
    virtual bool ready()=0;

    virtual ~SPI() = default;

};

class SPIFactory
{
public:
    static SPI *createInstance();
};
