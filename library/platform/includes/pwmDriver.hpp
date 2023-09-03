#pragma once

#include <stdint.h>

class PWMDriver
{
public:
    static PWMDriver *getInstance();

    virtual void write(uint8_t channel, uint16_t value, bool invert=false); // write to the pwm output buffer. channel starts at 0
    virtual void show() = 0;                                                // sends the output buffer to the pwm controller
    virtual bool ready() = 0;

    virtual void setFrequency(int Hz) = 0;               

    virtual ~PWMDriver() = default;

protected:
    PWMDriver(){}
};