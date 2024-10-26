#pragma once

#include "hyperion.hpp"
#include "MotorPosition.hpp"
#include <math.h>
#include <vector>

//Als de gordijnen niet helemaal goed aansluiten kan je hier extra overlap instellen
const float overlap = 0.05;

//Beide motoren hebben een bereik van 0 tot 0xFFFF
//0 is helemaal opgerold, 0xFFFF is helemaal afgerold


//map lfo position 0-1 to 0-0xFFFF
MotorPosition toTopPostion(float position, float width, float alpha)
{
    float overlapped = position - width + overlap;
    float constrained = Utils::constrain_f(overlapped, 0, 1);

    return MotorPosition(constrained * 0xFFFF, 255 * alpha);
}

//map lfo position 0-1 to 0xFFFF - 0
MotorPosition toBottomPostion(float position, float width, float alpha)
{
    float overlapped = position + width - overlap;
    float constrained = Utils::constrain_f(overlapped, 0, 1);

    return MotorPosition(0xFFFF - constrained * 0xFFFF, 255 * alpha);
}

namespace Window
{

    class SinPattern : public Pattern<MotorPosition>
    {
        LFO<NegativeCosFast> lfo;
        Transition transition = Transition(1000,1000);

    public:
        SinPattern()
        {
            this->name = "Sin";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
               return; // the fade out is done. we can skip calculating pattern data


            lfo.setPeriod(params->getVelocity(6000, 500));
            //lfo.setDutyCycle(params->getSize());
            int amount = 1; //params->getAmount(1, 3.99);

            for (int index = 0; index < width; index+=2)
            {
                pixels[index] = toTopPostion(lfo.getValue(amount * float(index) / width), 0.1, transition.getValue());
                pixels[index+1] = toBottomPostion(lfo.getValue(amount * float(index) / width), 0.1,  transition.getValue());
            }

        }
    };

    class SinGapPattern : public Pattern<MotorPosition>
    {
        LFO<NegativeCosFast> lfo;
        Transition transition = Transition(1000,1000);

    public:
        SinGapPattern()
        {
            this->name = "Sin gap";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod(params->getVelocity(6000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1, 3.99);

            for (int index = 0; index < width; index+=2)
            {
                float gap = 0.5*lfo.getValue(amount * float(index) / width);
                pixels[index] = toTopPostion(0.5,gap, transition.getValue()); 
                pixels[index+1] = toBottomPostion(0.5,gap, transition.getValue());
            }

        }
    };

    class FallingPattern : public Pattern<MotorPosition>
    {
        LFO<Tri> lfo;
        LFO<Square> lfoDir;
        Transition transition = Transition(1000,1000);

    public:
        FallingPattern()
        {
            this->name = "Falling";
        }

        inline void Calculate(MotorPosition *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            int period = params->getVelocity(6000, 500);
            float size = params->getSize();
            int amount = params->getAmount(1, 3.99);

            lfo.setPeriod(period);
            lfoDir.setPeriod(period);
            lfo.setDutyCycle(size);
            lfoDir.setDutyCycle(size);
            
            for (int index = 0; index < width; index+=2)
            {
                float gap = 0.1;
                float phase = amount * float(index) / width;
                float pos = lfo.getValue(phase);
                if (lfoDir.getValue(phase) > 0.5)
                {
                    gap = 0;
                }

                pixels[index]   = toTopPostion(   -gap+pos*(1+2*gap),gap, transition.getValue()); 
                pixels[index+1] = toBottomPostion(-gap+pos*(1+2*gap),gap, transition.getValue());
            }

        }
    };

}