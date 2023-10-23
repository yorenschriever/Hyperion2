#pragma once

#include <stdint.h>

class NeoPixels
{
public:
    struct Timing
    {
        int bit0_high;
        int bit0_low;
        int bit1_high;
        int bit1_low;
        int reset;
    };

    static NeoPixels *createInstance();

    virtual void begin(unsigned int pin, Timing timing = Kpbs800) = 0;
    virtual void send(uint8_t *buffer, int length) = 0;
    virtual bool ready() = 0;

    virtual ~NeoPixels() = default;

    static Timing WS2811;
    static Timing WS2812X;
    static Timing SK6812;
    static Timing Kpbs800;
    static Timing Kpbs400;
    static Timing APA106;

protected:
    NeoPixels() {}
};