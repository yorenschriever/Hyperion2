#pragma once

#include <stdint.h>

class SPI
{
public:
    virtual void begin() =0;
    virtual void send(uint8_t *buffer, int length)=0;
    virtual bool ready()=0;

    virtual ~SPI() = default;

};

class SPIFactory
{
public:
    static SPI *createInstance();
};
