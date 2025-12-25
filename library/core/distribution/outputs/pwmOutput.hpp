#pragma once

#include "pwmDriver.hpp"
#include "baseOutput.hpp"
#include "log.hpp"
#include <algorithm>

//This class controls the 12 pwm outputs on the back of the device
class PWMOutput final: public BaseOutput
{
    const char* TAG = "PWM";
public:

    PWMOutput() 
    {
    }

    //index and size are in bytes
    void setData(uint8_t *data, int size) override
    {
        if (!pwm) return;

        if (size%2!=0)
        {
            Log::error(TAG,"Cannot write an uneven number of bytes to a pwm output. Did you convert to Monochrome12 or RGB12?");
            return;
        }

        int requestedNumChannels = size/2;
        int copyEndChannel = std::min(12, requestedNumChannels);
        uint16_t* dataPtr = (uint16_t*) data;

        for (uint8_t channel=0; channel < copyEndChannel; channel++)
        {
            uint16_t value = *(dataPtr++);
            pwm->write(channel,value);
        }
    }

    bool ready() override
    {
        return pwm && pwm->ready();
    }

    void show() override
    {
        pwm->show();
    }

    void initialize() override
    {
        this->pwm = PWMDriver::getInstance();
    }

    void clear() override
    {
        if (!pwm) return;

        for (int i = 0; i < 12; i++)
            pwm->write(i,0);
    }

    void setLength(int len) override
    {
        //do nothing.
    }

private:
    PWMDriver *pwm = nullptr;
};