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


    bool ready() override
    {
        return pwm && pwm->ready();
    }


    void initialize() override
    {
        this->pwm = PWMDriver::getInstance();
    }

    void process(const Buffer& inputBuffer) override
    {
        if (!ready())
            return;

        if (inputBuffer.size() % 2 != 0)
        {
            Log::error(TAG, "Cannot write an uneven number of bytes to a pwm output. Did you convert to Monochrome12 or RGB12?");
            return;
        }

        int requestedNumChannels = inputBuffer.size() / 2;
        int copyEndChannel = std::min(12, requestedNumChannels);
        uint16_t* dataPtr = (uint16_t*) inputBuffer.data();

        for (uint8_t channel = 0; channel < copyEndChannel; channel++)
        {
            uint16_t value = *(dataPtr++);
            pwm->write(channel, value);
        }

        pwm->show();

        fpsCounter.increaseUsedFrameCount();
    }




private:
    PWMDriver *pwm = nullptr;
};