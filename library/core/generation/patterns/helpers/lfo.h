#pragma once

#include "log.hpp"
#include "utils.hpp"
#include <cmath>
#include <math.h>
#include <stdint.h>

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

class SawUp
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        return phase;
    }
};

class SawDown
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        return 1. - phase;
    }
};

class Tri
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        return abs(1 - 2 * phase);
    }
};

class Sin
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        return 0.5 + 0.5 * sin(phase * 2 * M_PI);
    }
};

class SinFast
{
    static float preCalc[256];
    static bool filled;

    static void fill()
    {
        for (int i = 0; i < 256; i++)
        {
            float phase = float(i) / 255;
            preCalc[i] = 0.5 + 0.5 * sin(phase * 2 * M_PI);
        }
        filled = true;
    }

public:
    static float getValue(float phase, float softEdgeWidth)
    {
        if (!filled)
            fill();
        return preCalc[int(phase * 255)];
    }

    friend class NegativeCosFast;
};
float SinFast::preCalc[256];
bool SinFast::filled = false;

// returns a sinus like value, in range 0-1 that start at 0. i.e.: -1*cos(phase)
class NegativeCosFast
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        if (!SinFast::filled)
            SinFast::fill();
        return SinFast::preCalc[int(phase * 255 + 256 - 256 / 4) % 256];
    }
};
#define Glow NegativeCosFast

class Cos
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        return 0.5 + 0.5 * cos(phase * 2 * M_PI);
    }
};

class Square
{
public:
    static float getValue(float phase, float pulse_width)
    {
        return phase < 0.5 ? 1 : 0;
    }
};

// PWM is a bit different: it is not a waveform, but always returns 1.
// You can use LFOs dutyCycle property to vary the pulse width
class PWM
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        if (phase == 1)
            return 0;
        return 1;
    }
};

class SoftPWM
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        if (phase == 1)
            return 0;
        if (phase < softEdgeWidth && softEdgeWidth > 0)
            return phase / softEdgeWidth;
        if (phase > 1. - softEdgeWidth && softEdgeWidth > 0)
            return (1. - phase) / softEdgeWidth;
        return 1;
    }
};

class SoftSawUp
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        if (phase > 1. - softEdgeWidth)
            return (1. - phase) / softEdgeWidth;
        if (softEdgeWidth == 1)
            return 0;
        return phase / (1 - softEdgeWidth);
    }
};

class SoftSawDown
{
public:
    static float getValue(float phase, float softEdgeWidth)
    {
        if (phase < softEdgeWidth && softEdgeWidth > 0)
            return phase / softEdgeWidth;
        return 1. - (phase - softEdgeWidth) / (1 - softEdgeWidth);
    }
};