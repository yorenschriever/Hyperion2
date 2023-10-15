#pragma once

#include <algorithm>
#include <cstring>
#include "input.hpp"
#include "platform/includes/utils.hpp"

//FallbackInput gets its input from one of the available sources
class FallbackInput : public Input
{

public:
    //input A, high prio input
    //input B, fallback
    //timeout: the delay before switching to B after a stops giving a signal, milliseconds
    FallbackInput(Input *inputA, Input *inputB, int timeout = 500)
    {
        this->inputA = inputA;
        this->inputB = inputB;
        this->timeout = timeout;
        this->aLastValid = 0;
    }

    virtual void begin() override
    {
        inputA->begin();
        inputB->begin();
    }

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize) override
    {
        int result = inputA->loadData(dataPtr, buffersize);
        if (result > 0)
        {
            aLastValid = Utils::millis();

            this->missedframecount = inputA->getMissedFrameCount();
            this->usedframecount = inputA->getUsedFrameCount();
            return result;
        }

        if (Utils::millis() - aLastValid > timeout){
            this->missedframecount = inputB->getMissedFrameCount();
            this->usedframecount = inputB->getUsedFrameCount();

            return inputB->loadData(dataPtr, buffersize);
        }

        return 0;
    }

    virtual void resetFrameCount() override{
        this->inputA->resetFrameCount();
        this->inputB->resetFrameCount();
    }

private:
    Input *inputA;
    Input *inputB;
    int timeout;
    unsigned long aLastValid;
};