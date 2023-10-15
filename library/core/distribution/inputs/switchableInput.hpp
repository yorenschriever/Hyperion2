#pragma once

#include <algorithm>
#include <cstring>
#include "input.hpp"

class SwitchableInput : public Input
{

public:
    using SwitchFunc = bool (*)(); 

    //input A, input when switchFunc evaluates to true
    //input B, fallback when switchFunc evaluates to false
    //SwitchFunc, function returning a boolean to select input A or B
    SwitchableInput(Input *inputA, Input *inputB, SwitchFunc switchFunc)
    {
        this->inputA = inputA;
        this->inputB = inputB;
        this->switchFunc = switchFunc;
    }

    virtual void begin() override
    {
        inputA->begin();
        inputB->begin();
    }

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize) override
    {
        return switchFunc() ? inputA->loadData(dataPtr, buffersize) : inputB->loadData(dataPtr, buffersize);
    }

    virtual void resetFrameCount() override{
        this->inputA->resetFrameCount();
        this->inputB->resetFrameCount();
    }

    virtual int getUsedFrameCount() override { return switchFunc() ? inputA->getUsedFrameCount() : inputB->getUsedFrameCount(); }
    virtual int getMissedFrameCount() override { return switchFunc() ? inputA->getMissedFrameCount() : inputB->getMissedFrameCount(); }
    virtual int getTotalFrameCount() override { return switchFunc() ? inputA->getTotalFrameCount() : inputB->getTotalFrameCount(); }

private:
    Input *inputA;
    Input *inputB;
    SwitchFunc switchFunc;
};