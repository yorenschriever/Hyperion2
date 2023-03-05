#pragma once

#include "output.hpp"

// nullOutput does not use the output.
// you can use this for performance testing or debugging
class NullOutput : public Output
{
public:
    NullOutput()
    {
    }

    // index and size are in bytes
    void setData(uint8_t *data, int size, int index) override {}

    bool ready() override
    {
        return true;
    }

    void show() override {}

    void postProcess() override {}

    void begin() override {}

    void clear() override {}

    // length is in bytes
    void setLength(int len) override {}

private:
};