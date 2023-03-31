
#pragma once

#include "utils.hpp"
#include <stdint.h>
#include <math.h>
#include <cmath>
#include "log.hpp"

template <class T>
class LFO
{

private:
    long starting_point = Utils::millis();
    float pulseWidth = 0.5;
    float skew = 1;
    int period;

public:
    LFO(int period=1000)
    {
        this->period = period;
        reset();
    }

    //LFO value between 0-1
    float getValue() { return getValue(0, period); }
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
        if (periodArg == 0)
            return 0;

        int deltaPhaseMs = periodArg * deltaPhase; 
        // Correction to make sure the phase never goes negative if deltaPhase > 1,
        // Not all plaforms handle modulus of negative numbers the same
        long correction = std::ceil(deltaPhase) * periodArg; 
        long phaseMs = (Utils::millis() - starting_point - deltaPhaseMs + correction) % periodArg;
        return float(phaseMs) / periodArg;
    }

    void reset()
    {
        this->starting_point = Utils::millis();
    }

    void setSkew(float skew)
    {
        this->skew = skew;
    }

    void setPulseWidth(float pulse_width)
    {
        this->pulseWidth = pulse_width;
    }

    int getPeriod() {
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
    static float getValue(float phase, float pulse_width)
    {
        return phase;
    }
};

class SawDown
{
public:
    static float getValue(float phase, float pulse_width)
    {
        return 1. - phase;
    }
};

class SawDownShort
{
public:
    static float getValue(float phase, float pulse_width)
    {
        if (phase > pulse_width)
            return 0;
        return 1. - phase/pulse_width;
    }
};

class Tri
{
public:
    static float getValue(float phase, float pulse_width)
    {
        return abs(1 - 2 * phase);
    }
};

class Sin
{
public:
    static float getValue(float phase, float pulse_width)
    {
        return 0.5 + 0.5 * sin(phase * 2 * M_PI);
    }
};

class SinFast
{
    static float preCalc[256];
    static bool filled;

    static void fill(){
        for (int i = 0; i < 256; i++)
        {
            float phase = float(i) / 255;
            preCalc[i] = 0.5 + 0.5 * sin(phase * 2 * M_PI);
        }
        filled = true;
    }

public:
    static float getValue(float phase, float pulse_width)
    {
        if (!filled) fill();
        return preCalc[int(phase * 255)];
    }

    friend class NegativeCosFast;
};
float SinFast::preCalc[256];
bool SinFast::filled = false;

//returns a sinus like value, in range 0-1 that start at 0. i.e.: -1*cos(phase)
class NegativeCosFast
{
public:
    static float getValue(float phase, float pulse_width)
    {
        if (!SinFast::filled) SinFast::fill();
        return SinFast::preCalc[int(phase * 255 + 256 - 256/4) % 256];
    }
};

class Cos
{
public:
    static float getValue(float phase, float pulse_width)
    {
        return 0.5 + 0.5 * cos(phase * 2 * M_PI);
    }
};

class PWM
{
public:
    static float getValue(float phase, float pulse_width)
    {
        return phase < pulse_width ? 1 : 0;
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

class SoftSquare
{
public:
    static float getValue(float phase, float pulse_width)
    {
        if (phase < pulse_width)
            return phase / pulse_width;
        if (phase < 0.5)
            return 1;
        if (phase < 0.5 + pulse_width)
            return 1 - (phase-0.5) / pulse_width;
        return 0;
    }
};

class SoftPWM
{
public:
    static float getValue(float phase, float pulse_width)
    {
        float softWidth = 0.1;
        if (phase < softWidth)
            return phase / softWidth;
        if (phase < pulse_width)
            return 1;
        if (phase < pulse_width + softWidth)
            return 1 - (phase - pulse_width) / softWidth;
        return 0;
    }
};

template <class INNER>
class LFOPause
{
public:
    static float getValue(float phase, float pulse_width)
    {
        if (phase > pulse_width)
            return 0;
        return INNER::getValue(phase/pulse_width,pulse_width);
    }
};