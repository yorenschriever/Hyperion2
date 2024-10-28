#pragma once

#include "log.hpp"
#include "utils.hpp"
#include <cmath>
#include <math.h>
#include <stdint.h>

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