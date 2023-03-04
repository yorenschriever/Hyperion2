
#pragma once

#include "utils.hpp"
#include <stdint.h>
#include <math.h>

template <class T>
class LFO
{

private:
    unsigned long startingpoint;
    float pulseWidth = 0.5;
    float skew = 1;

public:
    LFO(int period=0)
    {
        this->period = period;
        reset();
    }

    int period;

    //LFO value between 0-1
    float getValue() { return getValue(0, this->period); }
    float getValue(float deltaPhase) { return getValue(deltaPhase, period); }
    float getValue(float deltaPhase, int periodArg)
    {
        float phase = getPhase(deltaPhase, periodArg);

        if (skew != 1)
            phase = pow(phase, skew);

        return T::getValue(phase, pulseWidth);
    }

    //value between 0-1
    float getPhase() { return getPhase(0, period); }
    float getPhase(float deltaPhase, int periodArg)
    {
        int deltaPhaseMs = periodArg * deltaPhase;
        unsigned long phase = (Utils::millis() - startingpoint - deltaPhaseMs + periodArg) % periodArg;
        return ((float)phase / (float)periodArg);
    }

    void reset()
    {
        this->startingpoint = Utils::millis();
    }

    void setSkew(float skew)
    {
        this->skew = skew;
    }

    void setPulseWidth(float pulsewidth)
    {
        this->pulseWidth = pulsewidth;
    }

    int getPeriod() {
        return period;
    }

    void setPeriod(int newPeriod)
    {
        //correct the phase so the period change doesn't result in a phase change
        float phase = getPhase();
        this->startingpoint = Utils::millis() - phase * newPeriod;
        this->period = newPeriod;
    }
};

class SawUp
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        return phase;
    }
};

class SawDown
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        return 1. - phase;
    }
};

class SawDownShort
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        if (phase > pulsewidth)
            return 0;
        return 1. - phase/pulsewidth;
    }
};

class Tri
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        return abs(1 - 2 * phase);
    }
};

class Sin
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        return 0.5 + 0.5 * sin(phase * 2 * M_PI);
    }
};

class SinFast
{
    static float preCalc[256];
    static bool filled;

public:
    static float getValue(float phase, float pulsewidth)
    {
        if (!filled)
        {
            for (int i = 0; i < 256; i++)
            {
                float phase = float(i) / 255;
                preCalc[i] = 0.5 + 0.5 * sin(phase * 2 * M_PI);
            }
            filled = true;
        }

        return preCalc[int(phase * 255)];
    }
};
float SinFast::preCalc[256];
bool SinFast::filled = false;

class Cos
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        return 0.5 + 0.5 * cos(phase * 2 * M_PI);
    }
};

class PWM
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        return phase < pulsewidth ? 1 : 0;
    }
};

class Square
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        return phase < 0.5 ? 1 : 0;
    }
};

class SoftSquare
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        if (phase < pulsewidth)
            return phase / pulsewidth;
        if (phase < 0.5)
            return 1;
        if (phase < 0.5 + pulsewidth)
            return 1 - (phase-0.5) / pulsewidth;
        return 0;
    }
};

template <class INNER>
class LFOPause
{
public:
    static float getValue(float phase, float pulsewidth)
    {
        if (phase > pulsewidth)
            return 0;
        return INNER::getValue(phase/pulsewidth,pulsewidth);
    }
};