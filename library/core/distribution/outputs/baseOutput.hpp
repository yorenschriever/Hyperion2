#pragma once

#include <inttypes.h>
#include "../interfaces.hpp"
#include "../fpsCounter.hpp"

class Hyperion;

//You should derive from this class when writing your own output
class BaseOutput : public IOutput
{

public:
    bool ready() override = 0;
    void process(Buffer) override = 0;

    FPSCounter *getFpsCounter() override {
        return &fpsCounter;
    }

protected:
    FPSCounter fpsCounter;
};
