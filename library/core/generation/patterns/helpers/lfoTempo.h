#pragma once

#include "log.hpp"
#include "utils.hpp"
#include <cmath>
#include <math.h>
#include <stdint.h>
#include "lfoShapes.h"
#include "core/generation/patterns/helpers/tempo/tempo.h"

#define BEAT 1
#define MEASURE 4
#define PHRASE 16

template <class T>
class LFOTempo
{

private:
    float dutyCycle = 1;
    int period = 4; //period in number of beats

    // softEdgeWidth should always be really small. above 0.5 the lfo will have discontinuities.
    // softEdgeWidth can be used in combinations with soft shapes like
    // SoftPWM, SoftSawUp and SoftSawDown
    // It is an attempt you help anti-alias your pattern animations.
    // softEdgeWidth should therefore have a value similar to the smallest deltaPhase you use
    // eg: if you use getValue(float(i)/width), you should setSoftEdgeWidth(1./width)
    float softEdgeWidth = 0;

public:
    LFOTempo(int period = MEASURE, float dutyCycle = 1, float softEdgeWidth = 0)
    {
        this->period = period;
        this->dutyCycle = dutyCycle;
        this->softEdgeWidth = softEdgeWidth;
    }

    // LFO value between 0-1
    float getValue(float deltaPhase =0)
    {
        float phase = getPhase(deltaPhase);
        return T::getValue(phase, softEdgeWidth);
    }

    // phase value between 0-1
    float getPhase(float deltaPhase =0)
    {
        if (period == 0)
            return 0;

        float phase = Tempo::GetProgress(period); 
        int periodMs = Tempo::TimeBetweenBeats() * period;

        int deltaPhaseMs = periodMs * deltaPhase;
        // Correction to make sure the phase never goes negative if deltaPhase > 1,
        // Not all plaforms handle modulus of negative numbers the same
        long phaseMs = periodMs * phase;
        long correctionMs = std::ceil(deltaPhase) * periodMs;
        phaseMs = (phaseMs + deltaPhaseMs + correctionMs) % periodMs;

        // Log::info("LFOTempo", "LFOTempo: phase: %f, phaseMs: %d, periodMs: %d, deltaPhase: %f, deltaPhaseMs: %d", phase, phaseMs, periodMs, deltaPhase, deltaPhaseMs);

        int dutyCycleMs = periodMs * dutyCycle;
        if (phaseMs < dutyCycleMs)
            return float(phaseMs) / dutyCycleMs;
        else
            return 1;
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
        this->period = newPeriod;
    }

    //setPeriodExponential(0,4) = BEAT
    //setPeriodExponential(1,4) = MEASURE
    //setPeriodExponential(2,4) = PHRASE
    //example usage:
    //lfoTempo.setPeriodExponential((int)params->getVelocity(0,2.99)) 
    void setPeriodExponential(int value, int base=4)
    {
        this->period = pow(base, value);
    }
};
