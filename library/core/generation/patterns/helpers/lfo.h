#pragma once

#include "log.hpp"
#include "utils.hpp"
#include <cmath>
#include <math.h>
#include <stdint.h>
#include "lfoShapes.h"

template <class T>
class LFO
{

private:
    long starting_point = Utils::millis();
    float dutyCycle = 1;

    // softEdgeWidth should always be really small. above 0.5 the lfo will have discontinuities.
    // softEdgeWidth can be used in combinations with soft shapes like
    // SoftPWM, SoftSawUp and SoftSawDown
    // It is an attempt you help anti-alias your pattern animations.
    // softEdgeWidth should therefore have a value similar to the smallest deltaPhase you use
    // eg: if you use getValue(float(i)/width), you should setSoftEdgeWidth(1./width)
    float softEdgeWidth = 0;
    int period = 1000;

public:
    LFO(int period = 1000, float dutyCycle = 1, float softEdgeWidth = 0)
    {
        this->period = period;
        this->dutyCycle = dutyCycle;
        this->softEdgeWidth = softEdgeWidth;
        reset();
    }

    // LFO value between 0-1
    float getValue() { return getValue(0, period); }
    float getValue(float deltaPhase) { return getValue(deltaPhase, period); }
    float getValue(float deltaPhase, int periodArg)
    {
        float phase = getPhase(deltaPhase, periodArg);
        return T::getValue(phase, softEdgeWidth);
    }

    // phase value between 0-1
    float getPhase() { return getPhase(0, period); }
    float getPhase(float deltaPhase, int periodArg)
    {
        if (periodArg == 0)
            return 0;

        unsigned long now = Utils::millis();

#if ESP_PLATFORM
        // building with esp-idf gives me a compiler error:
        // internal compiler error: in extract_constrain_insn, at recog.cc:2692
        // Waiting for a fix will probably takes ages, because it needs to
        // be fixed in gcc, and then esp-idf needs to bump to the new version.
        // I came up with this workaround.
        // I narrowed this error down to using the millis() function here.
        // strangely, in here it is working, but not in Fade.
        // In Fade i added this modulus operator trick, and it solved the problem
        // Later the bug also occurred here in LFO, so i added the same trick.
        // The second operand of the operator is the biggest value
        // of unsigned long, so this is basically a no-op
        now = now % ((unsigned long)-1);
#endif

        int deltaPhaseMs = periodArg * deltaPhase;
        // Correction to make sure the phase never goes negative if deltaPhase > 1,
        // Not all plaforms handle modulus of negative numbers the same
        long correction = std::ceil(deltaPhase) * periodArg;
        long phaseMs = (now - starting_point - deltaPhaseMs + correction) % periodArg;

        int dutyCycleMs = periodArg * dutyCycle;
        if (phaseMs < dutyCycleMs)
            return float(phaseMs) / dutyCycleMs;
        else
            return 1;
    }

    void reset()
    {
        this->starting_point = Utils::millis();
    }

    void setDutyCycle(float dutyCycle)
    {
        this->dutyCycle = dutyCycle;
    }

    void setSoftEdgeWidth(float softEdgeWidth)
    {
        this->softEdgeWidth = softEdgeWidth;
    }

    int getPeriod()
    {
        return period;
    }

    void setPeriod(int newPeriod)
    {
        if (newPeriod == period)
            return;

        long ms = Utils::millis();
        long phaseMs = (ms - starting_point) % period;
        this->starting_point = ms - phaseMs * newPeriod / period;
        this->period = newPeriod;
    }
};
