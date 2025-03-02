#pragma once

#include <inttypes.h>
#include "../interfaces.hpp"
#include "../fpsCounter.hpp"

//You should derive from this class when writing your own output
class BaseOutput : public IOutput
{

public:

    // Repeat the functions from the interface for clarity
    void begin() override = 0;
    void clear() override = 0;
    bool ready() override = 0;
    void setLength(int length) override = 0;
    void setData(uint8_t *data, int size) override = 0;
    void show() override = 0;

    FPSCounter *getFpsCounter() override {
        return &fpsCounter;
    }

protected:
    FPSCounter fpsCounter;
};
