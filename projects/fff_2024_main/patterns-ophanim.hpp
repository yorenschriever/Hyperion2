#pragma once
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/interval.h"
#include "core/generation/patterns/helpers/timeline.h"
#include "core/generation/patterns/pattern.hpp"
#include "platform/includes/utils.hpp"
#include <math.h>
#include <vector>

// const int RINGSIZE = 500;

int remap(int i, int width)
{
    return i;

    // if (i < width/2) 
    //     return i;
    // return width+width/2-i-1;
}

namespace Ophanim
{

    class MonochromePattern : public Pattern<RGBA>
    {
    public:
        MonochromePattern()
        {
            this->name = "Monochrome";
        }

        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            RGBA col = params->getPrimaryColour() * transition.getValue();
            for (int i = 0; i < width; i++)
            {
                pixels[i] = col;
            }
        }
    };

    class StereochromePattern : public Pattern<RGBA>
    {
        int hoepel;
    public:
        StereochromePattern(int hoepel)
        {
            this->name = "Stereochrome";
            this->hoepel=hoepel;
        }

        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;


                RGBA col = ((hoepel % 2 == 0) ? params->getPrimaryColour() : params->getSecondaryColour()) * transition.getValue();
                for (int led = 0; led < width; led++)
                {
                    pixels[led] = col;
                }
            
        }
    };

    // class GradientPattern : public Pattern<RGBA>
    // {
    // public:
    //     GradientPattern()
    //     {
    //         this->name = "Gradient";
    //     }

    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         for (int hoepel = 0; hoepel < 2; hoepel++)
    //         {
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 RGBA col = params->getPrimaryColour() + (params->getSecondaryColour() * abs(float(led - RINGSIZE/2) / RINGSIZE/2));
    //                 pixels[hoepel * RINGSIZE + led] = col * transition.getValue();
    //             }
    //         }
    //     }
    // };

    // class Rainbow : public Pattern<RGBA>
    // {
    // public:
    //     Rainbow()
    //     {
    //         this->name = "Rainbow";
    //     }

    //     LFO<SawDown> lfo;
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         lfo.setPeriod(params->getVelocity(10000, 500));

    //         for (int hoepel = 0; hoepel < 2; hoepel++)
    //         {
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 RGBA col = Hue(led * 255 / RINGSIZE + lfo.getValue() * 255);
    //                 pixels[hoepel * RINGSIZE + led] = col * transition.getValue();
    //             }
    //         }
    //     }
    // };

    // class Rainbow2 : public Pattern<RGBA>
    // {
    // public:
    //     Rainbow2()
    //     {
    //         this->name = "Rainbow2";
    //     }

    //     LFO<SawDown> lfo;
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         lfo.setPeriod(params->getVelocity(10000, 500));

    //         for (int hoepel = 0; hoepel < 2; hoepel++)
    //         {
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 int hoepel2 = hoepel / 2;
    //                 RGBA col = Hue(hoepel2 * 255 / 5 + lfo.getValue() * 255);
    //                 pixels[hoepel * RINGSIZE + led] = col * transition.getValue();
    //             }
    //         }
    //     }
    // };

    // class Rainbow3 : public Pattern<RGBA>
    // {
    // public:
    //     Rainbow3()
    //     {
    //         this->name = "Rainbow3";
    //     }

    //     LFO<SawDown> lfo;
    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     const int hoepelmapping[10] = {0, 7,
    //                                    2, 9,
    //                                    4, 1,
    //                                    6, 3,
    //                                    8, 5};

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         lfo.setPeriod(params->getVelocity(10000, 500));

    //         for (int hoepel = 0; hoepel < 2; hoepel++)
    //         {
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 RGBA col = Hue(hoepelmapping[hoepel] * 255 / 10 + lfo.getValue() * 255);
    //                 pixels[hoepel * RINGSIZE + led] = col * transition.getValue();
    //             }
    //         }
    //     }
    // };

    // class HoepelsBase : public Pattern<RGBA>
    // {
    // public:
    //     HoepelsBase()
    //     {
    //         this->name = "Base";
    //     }

    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     BeatWatcher tempo;
    //     FadeDown fade[10] = {
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd),
    //         FadeDown(1400, WaitAtEnd)};
    //     int count = 0;

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         if (tempo.Triggered())
    //         {
    //             count = (count + 1) % 10;
    //             fade[count].reset();
    //         }

    //         for (int hoepel = 0; hoepel < 2; hoepel++)
    //         {
    //             // RGBA col = params->getPrimaryColour() * fade[hoepel].getValue() * transition.getValue();
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 RGBA col = params->getHighlightColour() * fade[hoepel].getValue(led * RINGSIZE) * transition.getValue();
    //                 pixels[hoepel * RINGSIZE + led] = col;
    //             }
    //         }
    //     }
    // };

    // class HoepelsTransition : public Pattern<RGBA>
    // {
    // public:
    //     HoepelsTransition()
    //     {
    //         this->name = "Transition";
    //     }

    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     BeatWatcher tempo;
    //     Transition chase[2] = {
    //         Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000),
    //         Transition(1050, Transition::fromSides, 500, 1050, Transition::fromCenter, 1000)};
    //     int count = 0;
    //     Permute perm = Permute(10);

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         for (int i = 0; i < 2; i++)
    //         {
    //             chase[i].Calculate(Tempo::GetBeatNumber() % 10 == perm.at[i]);
    //         }

    //         int hoepel=0;
    //         {
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 RGBA col = params->getPrimaryColour() * chase[hoepel].getValue(led, RINGSIZE) * transition.getValue();
    //                 pixels[hoepel * RINGSIZE + led] = col;
    //             }
    //         }
    //     }
    // };

    class SinStripPattern : public Pattern<RGBA>
    {
    public:
        SinStripPattern()
        {
            this->name = "Sinstrip";
        }

        const int numStrips = 10;
        LFO<Glow> lfo = LFO<Glow>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            // lfo.setPeriod(params->getVelocity(10000, 500));
            RGBA col = params->getSecondaryColour() * transition.getValue();

            lfo.setPeriod(params->getVelocity(10000,2000));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1,5);

            for (int led = 0; led < width; led++)
            {
                pixels[remap(led,width)] = col * lfo.getValue(amount *(float)led / width);
            }
            
        }
    };

    class SinStripPattern2 : public Pattern<RGBA>
    {
    public:
        SinStripPattern2()
        {
            this->name = "Sinstrip 2";
        }

        const int numStrips = 10;
        LFO<Glow> lfo = LFO<Glow>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(10000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1,5);

            RGBA col = params->getSecondaryColour() * transition.getValue();

            for (int led = 0; led < width; led++)
            {
                pixels[remap(led,width)] = col * lfo.getValue(amount * -1 * (float)led / width + (float)1 / 20);
            }
    
        }
    };

    // class FirePattern : public Pattern<RGBA>
    // {
    // public:
    //     FirePattern()
    //     {
    //         this->name = "Fire";
    //     }

    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     BeatWatcher tempo;
    //     Transition chase[2] = {
    //         Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150),
    //         Transition(150, Transition::fromCenter, 150, 150, Transition::fromCenter, 150)};
    //     int count = 0;
    //     Permute perm = Permute(10);
    //     Timeline timeline = Timeline();

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         timeline.FrameStart();

    //         if (!transition.Calculate(active))
    //             return;

    //         if (tempo.Triggered())
    //             timeline.reset();

    //         for (int i = 0; i < 2; i++)
    //         {
    //             chase[i].Calculate(timeline.GetTimelinePosition() < i * RINGSIZE);
    //         }

    //         for (int hoepel = 0; hoepel < 2; hoepel++)
    //         {
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 RGBA col = params->getSecondaryColour() * chase[hoepel].getValue(led, RINGSIZE) * transition.getValue();
    //                 pixels[hoepel * RINGSIZE + led] = col;
    //             }
    //         }
    //     }
    // };

    // class FireworkPattern : public Pattern<RGBA>
    // {
    // public:
    //     FireworkPattern()
    //     {
    //         this->name = "Firework";
    //     }

    //     Transition transition = Transition(
    //         200, Transition::none, 0,
    //         1000, Transition::none, 0);
    //     BeatWatcher tempo;
    //     FadeDown fade[2] = {FadeDown(400, WaitAtEnd), FadeDown(400, WaitAtEnd)};
    //     Permute perm[2] = {Permute(10), Permute(10)};

    //     void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    //     {
    //         if (!transition.Calculate(active))
    //             return;

    //         if (tempo.Triggered())
    //         {
    //             int which = abs(Tempo::GetBeatNumber()) % 2;
    //             fade[which].reset();
    //             perm[which].permute();
    //         }

    //         // RGBA baseCol = params->getHighlightColour() * transition.getValue();
    //         for (int hoepel = 0; hoepel < 2; hoepel++)
    //         {
    //             for (int led = 0; led < RINGSIZE; led++)
    //             {
    //                 float fadeValue = 0;
    //                 for (int i = 0; i < 2; i++)
    //                 {
    //                     int rotled = (perm[i].at[hoepel] * 1 + led) % RINGSIZE;
    //                     int mirrorled = abs(rotled - RINGSIZE/2);
    //                     fadeValue += fade[i].getValue(mirrorled * mirrorled / 0.5);
    //                 }
    //                 RGBA col = params->getGradientf(fadeValue) * fadeValue;
    //                 pixels[hoepel * 50 + led] = col;
    //             }
    //         }
    //     }
    // };

    class SinPattern : public Pattern<RGBA>
    {
    public:
        SinPattern()
        {
            this->name = "Sin";
        }

        const int numStrips = 10;
        LFO<SinFast> lfo = LFO<SinFast>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));

            for (int led = 0; led < width; led++)
                pixels[led] = params->getPrimaryColour() * lfo.getValue() * transition.getValue();
        }
    };

    class ChasePattern : public Pattern<RGBA>
    {
    public:
        ChasePattern()
        {
            this->name = "Chase";
        }

        const int numStrips = 10;
        LFO<SoftSawDown> lfo = LFO<SoftSawDown>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1,5);
            lfo.setSoftEdgeWidth(amount * 1./50);
            
            RGBA col = params->getPrimaryColour() * transition.getValue();

            float dir = 1; // hoepel % 2 ==0 ? 1 : -1;
            for (int led = 0; led < width; led++)
            {
                pixels[remap(led,width)] = col * lfo.getValue(amount * dir * (float)led / width);
            }
            
        }
    };

    class AntichasePattern : public Pattern<RGBA>
    {
    public:
        AntichasePattern()
        {
            this->name = "Anti chase";
        }

        const int numStrips = 10;
        LFO<SoftSawDown> lfo = LFO<SoftSawDown>(5000);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1,3);
            lfo.setSoftEdgeWidth(amount * 1./50);
            RGBA col = params->getSecondaryColour() * transition.getValue();

            int hoepel=0;
            float dir = hoepel % 2 == 0 ? 1 : -1;
            for (int led = 0; led < width; led++)
            {
                pixels[led] = col * lfo.getValue(amount * dir * (float)led / width);
            }
            
        }
    };

    template <class T>
    class ClivePattern : public Pattern<RGBA>
    {
    public:
        ClivePattern()
        {
            this->name = "Clive";
        }

        int numSegments;
        int averagePeriod;
        float precision;
        LFO<T> lfo;
        Permute perm;
        int color;

        ClivePattern(int color = 0, int numSegments = 10, int averagePeriod = 1000, float precision = 1, float pulsewidth = 1, float softEdgeWidth = 0)
        {
            this->numSegments = std::max(numSegments, 1);
            this->averagePeriod = averagePeriod;
            this->precision = precision;
            this->perm = Permute(numSegments);
            this->lfo.setDutyCycle(pulsewidth);
            this->lfo.setSoftEdgeWidth(softEdgeWidth);
            this->color = color;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            auto col = params->getPrimaryColour();
            if (color == 1)
                col = params->getSecondaryColour();
            if (color == 2)
                col = params->getHighlightColour();

            for (int index = 0; index < width; index++)
            {
                int permutedQuantized = perm.at[index * numSegments / width] * width / numSegments;
                int interval = averagePeriod + permutedQuantized * (averagePeriod * precision) / width;
                pixels[index] += col * lfo.getValue(0, interval);
            }
        }
    };

    class SquareGlitchPattern : public Pattern<RGBA>
    {
    public:
        SquareGlitchPattern()
        {
            this->name = "Square glitch";
        }

        Timeline timeline = Timeline(10);
        Permute perm = Permute(100);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            perm.permute();

            for (int index = 0; index < width; index++)
            {
                int quantized = index / 10;
                if (perm.at[quantized] > 25)
                    continue;
                pixels[index] += params->getSecondaryColour() * transition.getValue();
            }
        }
    };

    class OnbeatFadeAllPattern : public Pattern<RGBA>
    {
    public:
        OnbeatFadeAllPattern()
        {
            this->name = "Onbeat Fade All";
        }

        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade = FadeDown(1400, WaitAtEnd);
        BeatWatcher tempo = BeatWatcher();

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;
            if (tempo.Triggered())
                fade.reset();

            fade.setDuration(params->getVelocity(2000,200));

            RGBA col = params->getPrimaryColour() * fade.getValue() * transition.getValue();
            for (int i = 0; i < width; i++)
                pixels[i] += col;
        }
    };

    class OnbeatFadePattern : public Pattern<RGBA>
    {
    public:
        OnbeatFadePattern()
        {
            this->name = "Onbeat Fade";
        }

        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[5] = {
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd),
            FadeDown(1400, WaitAtEnd)};
        BeatWatcher tempo = BeatWatcher();
        int pos = 0;

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            if (tempo.Triggered())
            {
                pos = (pos + 1 + Utils::random(0, 4)) % 5;
                fade[pos].reset();
            }

            for (int i = 0; i < 5; i++)
            {
                fade[i].setDuration(params->getVelocity(2000,200));
                RGBA col = params->getSecondaryColour() * fade[i].getValue() * transition.getValue();
                for (int j = 0; j < 100; j++)
                    pixels[i * 100 + j] += col;
            }
        }
    };

    class SlowStrobePattern : public Pattern<RGBA>
    {
    public:
        SlowStrobePattern()
        {
            this->name = "Slow Strobe";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            RGBA col = Utils::millis() % 100 < 25 ? params->getPrimaryColour() : RGBA();

            for (int index = 0; index < width; index++)
                pixels[index] = col;
        }
    };

    class GlowPulsePattern : public Pattern<RGBA>
    {
    public:
        GlowPulsePattern()
        {
            this->name = "Glow Pulse";
        }

        Permute perm;
        LFO<SinFast> lfo = LFO<SinFast>(10000);
        Transition transition;

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod(500 + 10000 * (1.0f - params->getVelocity()));
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                float val = 1.025 * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000);
                if (val < 1.0)
                    pixels[perm.at[index]] = params->getSecondaryColour() * lfo.getValue(float(Transition::fromCenter(index, width, 1000)) / -1000) * transition.getValue(index, width);
                else
                    pixels[perm.at[index]] = (params->getSecondaryColour() + (params->getHighlightColour() * (val - 1) / 0.025)) * transition.getValue(index, width);
            }
        }
    };


class SinChaseMaskPattern : public Pattern<RGBA>
    {

        Transition transition;
        LFO<Glow> lfo;

    public:
        SinChaseMaskPattern()
        {
            this->name = "Sin chase Mask";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setDutyCycle(params->getSize(0.1, 1));
            lfo.setPeriod(params->getVelocity(4000, 500));
            int amount = params->getAmount(1, 3);
            float offset = params->getOffset(0, 5);

            for (int i = 0; i < width; i++)
            {
                float phase = ((float)i / width) * amount * 2 + float(i % (width / 2)) * offset / width;
                pixels[remap(i,width)] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(phase)) * transition.getValue();
            }
        }
    };

    class GlowPulseMaskPattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;

    public:
        GlowPulseMaskPattern()
        {
            this->name = "Glow pulse mask";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            float amount = params->getAmount(0.005, 0.1);
            lfo.setPeriod(params->getVelocity(3000, 500) / amount);
            lfo.setDutyCycle(amount);
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                pixels[perm.at[index]] = RGBA(0, 0, 0, 255) * (1. - lfo.getValue(float(index) / width)) * transition.getValue(index, width);
            }
        }
    };

}