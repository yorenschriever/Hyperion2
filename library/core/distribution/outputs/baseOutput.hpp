#pragma once

#include <inttypes.h>
#include "../interfaces.hpp"
#include "../fpsCounter.hpp"

class Hyperion;

//You should derive from this class when writing your own output
class BaseOutput : public IOutput
{

public:
    // Repeat the functions from the interface for clarity
    void initialize() override {};
    void clear() override = 0;
    bool ready() override = 0;
    void setLength(int length) override = 0;
    void setData(uint8_t *data, int size) override = 0;
    void show() override = 0;

    FPSCounter *getFpsCounter() override {
        return &fpsCounter;
    }

    void registerToHyperion(IRegistrant *hyp) override {
        hyp->addOutput(this);
        hyp->addInitializer(this);
    }
protected:
    FPSCounter fpsCounter;
};
