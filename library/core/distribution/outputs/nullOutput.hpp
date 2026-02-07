#pragma once

#include "baseOutput.hpp"

// nullOutput does not use the output.
// you can use this for performance testing or debugging
class NullOutput final: public BaseOutput
{
public:
    NullOutput() {}

    bool ready() override {return true;};
    void initialize() override {};

    void process(const Buffer&) override {
        fpsCounter.increaseUsedFrameCount();
    };
};