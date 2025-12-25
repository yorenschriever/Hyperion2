#pragma once

#include "baseOutput.hpp"

// nullOutput does not use the output.
// you can use this for performance testing or debugging
class NullOutput final: public BaseOutput
{
public:
    NullOutput() {}

    void clear() override {};
    bool ready() override {return true;};
    void setLength(int length) override {};
    void setData(uint8_t *data, int size) override {};
    void show() override {};
};