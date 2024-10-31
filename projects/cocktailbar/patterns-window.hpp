#pragma once

#include "MotorPosition.hpp"
#include "hyperion.hpp"
#include <math.h>
#include <vector>

const int transitionTime = 2500;

// Als de gordijnen niet helemaal goed aansluiten kan je hier extra overlap instellen
const float overlap = 0.05;

// Beide motoren hebben een bereik van 0 tot 0xFFFF
// 0 is helemaal opgerold, 0xFFFF is helemaal afgerold

// map lfo position 0-1 to 0-0xFFFF
MotorPosition toTopPostion(float position, float width, float alpha)
{
    float overlapped = position - width + overlap;
    float constrained = Utils::constrain_f(overlapped, 0, 1);

    return MotorPosition(constrained * 0xFFFF, 255 * alpha);
}

// map lfo position 0-1 to 0xFFFF - 0
MotorPosition toBottomPostion(float position, float width, float alpha)
{
    float overlapped = position + width - overlap;
    float constrained = Utils::constrain_f(overlapped, 0, 1);

    return MotorPosition(0xFFFF - constrained * 0xFFFF, 255 * alpha);
}

float addGapMargin(float pos, float gap)
{
    return -gap + pos * (1 + 2 * gap);
}

namespace Window
{

    class StaticPattern : public Pattern<MotorPosition>
    {
        Transition transition = Transition(transitionTime, transitionTime);
        float top;
        float bottom;

    public:
        StaticPattern(const char *name, float top, float bottom)
        {
            this->name = name;
            this->top = top;
            this->bottom = bottom;
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index += 2)
            {
                pixels[index] = toTopPostion(top, 0, transition.getValue());
                pixels[index + 1] = toBottomPostion(bottom, 0, transition.getValue());
            }
        }
    };

    class SinPattern : public Pattern<MotorPosition>
    {
        LFO<NegativeCosFast> lfo;
        Transition transition = Transition(transitionTime, transitionTime);

    public:
        SinPattern()
        {
            this->name = "Sin";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float velocity = params->getVelocity(20000, 2000);
            float intensity = params->getIntensity();

            float gap = params->getSize(0.05, 0.5);
            float amount = params->getAmount(1, 3.99);

            lfo.setDutyCycle(intensity);
            lfo.setPeriod(velocity / intensity * amount);

            for (int index = 0; index < width; index += 2)
            {
                float pos = lfo.getValue(amount * float(index) / width);
                float pos2 = addGapMargin(pos, gap);
                pixels[index] = toTopPostion(pos2, gap, transition.getValue());
                pixels[index + 1] = toBottomPostion(pos2, gap, transition.getValue());
            }
        }
    };

    class SinGapPattern : public Pattern<MotorPosition>
    {
        LFO<NegativeCosFast> lfo;
        Transition transition = Transition(transitionTime, transitionTime);

    public:
        SinGapPattern()
        {
            this->name = "Sin gap";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            // lfo.setPeriod(params->getVelocity(6000, 500));
            // lfo.setDutyCycle(params->getSize());
            // int amount = params->getAmount(1, 3.99);

            float velocity = params->getVelocity(20000, 2000);
            float intensity = params->getIntensity();

            float gap = params->getSize(0.05, 0.5);
            float amount = params->getAmount(1, 3.99);

            lfo.setDutyCycle(intensity);
            lfo.setPeriod(velocity / intensity * amount);

            for (int index = 0; index < width; index += 2)
            {
                float gap = 0.5 * lfo.getValue(amount * float(index) / width);
                pixels[index] = toTopPostion(0.5, gap, transition.getValue());
                pixels[index + 1] = toBottomPostion(0.5, gap, transition.getValue());
            }
        }
    };

    class FallingPattern : public Pattern<MotorPosition>
    {
        LFO<Tri> lfo;
        LFO<Square> lfoDir;
        Transition transition = Transition(transitionTime, transitionTime);

    public:
        FallingPattern()
        {
            this->name = "Falling";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            // int period = params->getVelocity(20000, 2000);
            // float size = params->getSize();
            // int amount = params->getAmount(1, 3.99);

            // lfo.setPeriod(period / size * amount);
            // lfoDir.setPeriod(period / size * amount);
            // lfo.setDutyCycle(size);
            // lfoDir.setDutyCycle(size);

            float velocity = params->getVelocity(20000, 2000);
            float intensity = params->getIntensity();

            float gap = params->getSize(0.05, 0.5);
            float amount = params->getAmount(1, 3.99);

            lfo.setDutyCycle(intensity);
            lfo.setPeriod(velocity / intensity * amount);
            lfoDir.setDutyCycle(intensity);
            lfoDir.setPeriod(velocity / intensity * amount);

            for (int index = 0; index < width; index += 2)
            {
                float gap2 = gap;
                float phase = amount * float(index) / width;
                float pos = 1. - lfo.getValue(phase);
                if (lfoDir.getValue(phase) < 0.5)
                {
                    gap2 = 0;
                }

                pixels[index] = toTopPostion(addGapMargin(pos, gap2), gap2, transition.getValue());
                pixels[index + 1] = toBottomPostion(addGapMargin(pos, gap2), gap2, transition.getValue());
            }
        }
    };

    class SinTempoPattern : public Pattern<MotorPosition>
    {
        LFOTempo<NegativeCosFast> lfo;
        Transition transition = Transition(transitionTime, transitionTime);

    public:
        SinTempoPattern()
        {
            this->name = "Sin tempo";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            // float velocity = params->getVelocity(20000, 2000);
            // float intensity = params->getIntensity();

            // float gap = params->getSize(0.05, 0.5);
            // float amount = params->getAmount(1, 3.99);
            float amount = 1;
            float gap = 0.25;

            // lfo.setDutyCycle(intensity);
            // lfo.setPeriod(velocity / intensity * amount);

            lfo.setPeriodExponential((int)params->getVelocity(6.99, 3), 2);
            // lfo.setPeriod(1000);
            // lfo.setPeriod(1);

            for (int index = 0; index < width; index += 2)
            {
                float pos = lfo.getValue(amount * float(index) / width);
                float pos2 = addGapMargin(pos, gap);
                pixels[index] = toTopPostion(pos2, gap, transition.getValue());
                pixels[index + 1] = toBottomPostion(pos2, gap, transition.getValue());
            }
        }
    };

    class GenericFallingPattern : public Pattern<MotorPosition>
    {
        LFOTempo<Tri> lfo;
        LFOTempo<Square> lfoDir;
        Transition transition = Transition(transitionTime, transitionTime);
        int amount;

    public:
        GenericFallingPattern(const char *name, float dutyCycle = 1, int amount = 1, int period = PHRASE)
        {
            this->name = name;
            this->amount = amount;
            lfo.setPeriod(period);
            lfo.setDutyCycle(dutyCycle);
            lfoDir.setPeriod(period);
            lfoDir.setDutyCycle(dutyCycle);
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float intensity = params->getIntensity(0.05, 0.25);

            for (int index = 0; index < width; index += 2)
            {
                float phase = amount * float(index/2) / width;
                float pos = 1. - lfo.getValue(phase);
                float gap = (lfoDir.getValue(phase) > 0.5) ? intensity : 0;

                pixels[index] = toTopPostion(addGapMargin(pos, gap), gap, transition.getValue());
                pixels[index + 1] = toBottomPostion(addGapMargin(pos, gap), gap, transition.getValue());
            }
        }
    };

    template <class T>
    class GenericPositionLFOPattern : public Pattern<MotorPosition>
    {
        LFOTempo<T> lfo;
        Transition transition = Transition(transitionTime, transitionTime);
        int amount;

    public:
        GenericPositionLFOPattern(const char *name, float dutyCycle = 1, int amount = 1, int period = PHRASE)
        {
            this->name = name;
            this->amount = amount;
            lfo.setPeriod(period);
            lfo.setDutyCycle(dutyCycle);
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float gap = params->getIntensity(0.05, 0.25);

            for (int index = 0; index < width; index += 2)
            {
                float phase = amount * float(index/2) / width;
                float pos = lfo.getValue(phase);

                // pixels[index] = toTopPostion(addGapMargin(pos, gap), gap, transition.getValue());
                // pixels[index + 1] = toBottomPostion(addGapMargin(pos, gap), gap, transition.getValue());
                pixels[index] = toTopPostion(pos, gap, transition.getValue());
                pixels[index + 1] = toBottomPostion(pos, gap, transition.getValue());
            }
        }
    };

    template <class T>
    class GenericGapLFOPattern : public Pattern<MotorPosition>
    {
        LFOTempo<T> lfo;
        Transition transition = Transition(transitionTime, transitionTime);
        float amount;

    public:
        GenericGapLFOPattern(const char *name, float dutyCycle = 1, float amount = 1, int period = PHRASE)
        {
            this->name = name;
            this->amount = amount;
            lfo.setPeriod(period);
            lfo.setDutyCycle(dutyCycle);
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data


            for (int index = 0; index < width; index += 2)
            {
                float phase = amount * float(index/2) / width;
                float gap = lfo.getValue(phase) * params->getIntensity(0.05, 0.25);
                float pos=0.5;

                // pixels[index] = toTopPostion(addGapMargin(pos, gap), gap, transition.getValue());
                // pixels[index + 1] = toBottomPostion(addGapMargin(pos, gap), gap, transition.getValue());
                pixels[index] = toTopPostion(pos, gap, transition.getValue());
                pixels[index + 1] = toBottomPostion(pos, gap, transition.getValue());
            }
        }
    };
}