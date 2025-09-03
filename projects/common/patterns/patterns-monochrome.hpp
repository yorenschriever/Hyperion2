#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace MonochromePatterns
{

    class SinPattern : public Pattern<Monochrome>
    {
        LFO<NegativeCosFast> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        SinPattern()
        {
            this->name = "Sin";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setPeriod(params->getVelocity(6000, 500));
            lfo.setDutyCycle(params->getSize());
            int amount = params->getAmount(1, 3.99);

            for (int index = 0; index < width; index++)
                pixels[index] = Monochrome(255 * lfo.getValue(amount * float(index) / width)) * transition.getValue(index, width);
        }
    };

    class GlowPattern : public Pattern<Monochrome>
    {
        Permute perm;
        LFO<Glow> lfo;
        Transition transition = Transition(
            1000, Transition::none, 0,
            2000, Transition::none, 0);

    public:
        GlowPattern(int direction = 1)
        {
            this->name = "Glow";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            perm.setSize(width);

            float velocity = params->getVelocity(6000, 500);
            float amount = params->getAmount();
            lfo.setDutyCycle(amount);
            lfo.setPeriod(velocity / amount);

            for (int index = 0; index < width; index++)
                pixels[perm.at[index]] = Monochrome(255 * lfo.getValue(float(index) / width)) * transition.getValue(index, width);
        }
    };

    class FastStrobePattern : public Pattern<Monochrome>
    {
        Transition transition = Transition(
            1000, Transition::none, 0,
            0, Transition::none, 0);

    public:
        FastStrobePattern()
        {
            this->name = "Fast strobe";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            Monochrome value = Monochrome(Utils::millis() % 60 < 25 ? 255 : 0) * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = pixels[index] * (1. - transition.getValue()) + value;
        }
    };

    class SlowStrobePattern : public Pattern<Monochrome>
    {
    public:
        SlowStrobePattern()
        {
            this->name = "Slow strobe";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
                pixels[index] = Utils::millis() % 200 < 25 ? 255 : 0;
        }
    };

    class FastStrobePattern2 : public Pattern<Monochrome>
    {

        int framecounter = 1;

    public:
        FastStrobePattern2()
        {
            this->name = "Fast Strobe 2";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            framecounter--;

            Monochrome color = 0;
            if (framecounter <= 1)
                color = 255;

            if (framecounter == 0)
                framecounter = 5; // Params::getVelocity(40,4);

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class BlinderPattern : public Pattern<Monochrome>
    {
    public:
        BlinderPattern(FadeShape in = Transition::none, FadeShape out = Transition::none, uint8_t intensity = 255, int fadein = 200, int fadeout = 600)
        {
            transition = Transition(
                fadein, in, 200,
                fadeout, out, 500);
            this->intensity = intensity;
            this->name = "Blinder";
        }

    protected:
        Transition transition;
        uint8_t intensity;

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
                pixels[index] = Monochrome(intensity) * transition.getValue(index, width);
        }
    };

    class BeatAllFadePattern : public Pattern<Monochrome>
    {
    public:
        BeatAllFadePattern()
        {
            this->name = "Beat all fade";
        }

    protected:
        BeatWatcher watcher = BeatWatcher();
        FadeDown fader = FadeDown();

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            fader.setDuration(params->getSize(100, 1000));

            if (active && watcher.Triggered())
                fader.reset();

            for (int index = 0; index < width; index++)
                pixels[index] = 255 * fader.getValue();
        }
    };

    class BeatShakePattern : public Pattern<Monochrome>
    {
        BeatWatcher watcher = BeatWatcher();
        FadeDown fader = FadeDown(300, WaitAtEnd);
        Permute perm = Permute(0);

    public:
        BeatShakePattern()
        {
            this->name = "Beat shake";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active && fader.getValue(width * 10) == 0)
            {
                watcher.Triggered();
                return;
            }

            perm.setSize(width);
            fader.setDuration(params->getSize(100, 400));

            if (active && watcher.Triggered())
            {
                perm.permute();
                fader.reset();
            }

            for (int index = 0; index < width; index++)
                pixels[index] = 255 * fader.getValue(perm.at[index] * 10);
        }
    };

    class BeatSingleFadePattern : public Pattern<Monochrome>
    {
        BeatWatcher watcher = BeatWatcher();
        FadeDown fader = FadeDown(200, WaitAtEnd);
        int current = 0;

    public:
        BeatSingleFadePattern()
        {
            this->name = "Beat single fade";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            fader.setDuration(params->getSize(100, 1000));

            if (active && watcher.Triggered())
            {
                current = Utils::random(0, width);
                fader.reset();
            }

            for (int index = 0; index < width; index++)
                pixels[index] = index == current ? 255 * fader.getValue() : 0;
        }
    };

    class BeatMultiFadePattern : public Pattern<Monochrome>
    {
    public:
        BeatMultiFadePattern(int duration = 200)
        {
            fader.duration = duration;
            this->name = "Beat multi fade";
        }

        BeatWatcher watcher = BeatWatcher();
        FadeDown fader = FadeDown(200, WaitAtEnd);
        Permute perm = Permute(0);

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active && fader.getValue() == 0)
            {
                watcher.Triggered();
                return;
            }

            perm.setSize(width);
            fader.setDuration(params->getSize(100, 1000));

            if (active && watcher.Triggered())
            {
                perm.permute();
                fader.reset();
            }

            for (int index = 0; index < width; index++)
                pixels[index] = perm.at[index] < width / 2 ? 255 * fader.getValue() : 0;
        }
    };

    class GlitchPattern : public Pattern<Monochrome>
    {
        Timeline timeline = Timeline(50);
        Permute perm = Permute(0);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        GlitchPattern()
        {
            this->name = "Glitch";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            perm.setSize(width);

            if (timeline.Happened(0))
                perm.permute();

            uint8_t val = timeline.GetTimelinePosition() < 25 ? 255 * transition.getValue() : 0;

            for (int index = 0; index < width / 2; index++)
                pixels[perm.at[index]] = Monochrome(val);
        }
    };

    class OnPattern : public Pattern<Monochrome>
    {
        uint8_t intensity;
        
        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

    public:
        OnPattern(uint8_t intensity = 180, const char *name = "on")
        {
            this->intensity = intensity;
            this->name = name;
        }
        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            Monochrome value = Monochrome(intensity) * transition.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = value;
        }
    };

    class StaticPattern : public Pattern<Monochrome>
    {
        struct Channel {
            int channel;
            uint8_t intensity = 255;
        };

        std::vector<Channel> channels;
        Transition transition;

    public:
        StaticPattern(const char *name, std::vector<Channel> channels, int fadein = 0, int fadeout = 0)
        {
            this->channels = channels;
            this->name = name;
            this->transition = Transition(fadein, fadeout);
        }
        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (auto &channel : channels){
                if (channel.channel >= width) continue;
                pixels[channel.channel] = Monochrome(channel.intensity) * transition.getValue();
            }
        }
    };

    class TimelinePattern : public Pattern<Monochrome>
    {
        struct Channel {
            int channel;
            uint8_t intensity = 255;
            int on = 0;
            int off = 1000;
        };

        std::vector<Channel> channels;
        Timeline timeline;

    public:
        TimelinePattern(const char *name, std::vector<Channel> channels)
        {
            this->channels = channels;
            this->name = name;
        }
        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active){
                timeline.reset();
                return; 
            }

            timeline.FrameStart();

            for (auto &channel : channels){
                if (channel.channel >= width) continue;
                if (!(timeline.Happened(channel.on) && !timeline.Happened(channel.off))) continue;
    
                pixels[channel.channel] = Monochrome(channel.intensity);
            }
        }
    };


    template <class T>
    class LFOPattern : public Pattern<Monochrome>
    {
        LFO<T> lfo;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        LFOPattern(std::string name = "LFO")
        {
            this->name = name;
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float velocity = params->getVelocity(6000, 500);
            int amount = params->getAmount(1, 3.99);
            float size = params->getSize(0.1, 1);

            lfo.setPeriod(velocity / amount);
            lfo.setDutyCycle(size);

            for (int index = 0; index < width; index++)
            {
                pixels[index] = 255 * transition.getValue() * lfo.getValue((float)amount * index / width);
            }
        }
    };

    class BeatStepPattern : public Pattern<Monochrome>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        BeatStepPattern()
        {
            this->name = "Beat step";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int steps = params->getVelocity(1, 4);
            // int amount = params->getAmount(1, 4);

            for (int index = 0; index < width; index++)
            {
                if ((index * steps) == (Tempo::GetBeatNumber()) % (width * steps))
                    pixels[index] = 255 * transition.getValue();
            }
        }
    };

    class IntervalPattern : public Pattern<Monochrome>
    {
        LFOTempo<PWM> lfo;
        
    public:
        IntervalPattern(int duty, int period, const char *name = "on")
        {
            this->name = name;
            lfo.setDutyCycle((float)duty / period);
            lfo.setPeriod(period);
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int index = 0; index < width; index++)
                pixels[index] = 255 * lfo.getValue();
        }
    };

    class SingleGlitchPattern : public Pattern<Monochrome>
    {
        int last = 0;

    public:
        SingleGlitchPattern()
        {
            this->name = "Single glitch";
        }

        inline void Calculate(Monochrome *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            int index = (last + Utils::random(1, width-1)) % width;
            pixels[last] = 255;
            last=index;
        }
    };

}