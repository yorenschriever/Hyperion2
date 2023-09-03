#pragma once

#include "pwmDriver.hpp"
#include "output.hpp"
#include "log.hpp"
#include <algorithm>

//This class controls the 12 pwm outputs on the back of the device
class PWMOutput : public Output
{
    const char* TAG = "PWM";
public:

    //startchannel 1-12
    PWMOutput(uint8_t startChannel=1) 
    {
        this->startChannel = startChannel-1;
    }

    //index and size are in bytes
    void setData(uint8_t *data, int size, int index) override
    {
        if (!pwm) return;

        if (index%2!=0)
        {
            Log::error(TAG,"Cannot write to an uneven byte index. Did you convert to Monochrome12 or RGB12?");
            return;
        }

        if (size%2!=0)
        {
            Log::error(TAG,"Cannot write an uneven number of bytes to a pwm output. Did you convert to Monochrome12 or RGB12?");
            return;
        }

        int copyStartChannel = startChannel + index/2;
        int requestedNumChannels = size/2;
        int copyEndChannel = std::min(12, copyStartChannel + requestedNumChannels);
        uint16_t* dataPtr = (uint16_t*) data;

        for (uint8_t channel=copyStartChannel; channel < copyEndChannel; channel++)
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
        //set this static variable to indicate that any dmx instance has updated the data
        groupDirty = true;
    }

    void postProcess() override
    {
        //read the static variable to see if any instance has updated the data
        if (!groupDirty || !pwm)
            return;

        pwm->show();
        groupDirty = false;
    }

    void begin() override
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
    PWMDriver *pwm;
    uint8_t startChannel;
    static bool groupDirty;
};

bool PWMOutput::groupDirty = false;