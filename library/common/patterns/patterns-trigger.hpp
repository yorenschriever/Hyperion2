#pragma once
#include "hyperion.hpp"
#include <math.h>
#include <vector>

namespace TriggerPatterns
{

    class FadingNoisePattern : public Pattern<RGBA>
    {
        Fade<Down, Cubic> fade = Fade<Down, Cubic>(600);
        Permute perm;
        bool lastActive;

    public:
        FadingNoisePattern()
        {
            this->name = "Fading noise";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            if (fade.getPhase() == 1)
                return;

            perm.setSize(width);
            perm.permute();

            int noiseLevel = fade.getValue() * width / 3;
            for (int index = 0; index < noiseLevel; index++)
            {
                pixels[perm.at[index]] = params->getHighlightColor();
            }
        }
    };

    class PulsePattern : public Pattern<RGBA>
    {
        FadeDown fade;
        bool lastActive;

    public:
        PulsePattern()
        {
            this->name = "Pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            if (fade.getPhase() == 1)
                return;

            fade.setDuration(params->getVelocity(200, 20));

            RGBA color = params->getSecondaryColor() * fade.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class SlowPulsePattern : public Pattern<RGBA>
    {
        FadeDown fade;
        bool lastActive;

    public:
        SlowPulsePattern()
        {
            this->name = "Slow Pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            if (fade.getPhase() == 1)
                return;

            fade.setDuration(params->getVelocity(3000, 500));

            RGBA color = params->getPrimaryColor() * fade.getValue();

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class GrowingCirclePattern : public Pattern<RGBA>
    {
        FadeDown fade;
        PixelMap::Polar *map;
        bool lastActive;

    public:
        GrowingCirclePattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Growing circles";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (active && !lastActive)
                fade.reset();
            lastActive = active;

            float velocity = params->getVelocity(1000, 100);
            fade.duration = params->getSize(400, 50) * velocity / 1000;

            if (fade.isFinished(1 * velocity))
                return;

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue(map->r(i) * velocity);
                RGBA color = params->getPrimaryColor();
                pixels[i] = color * fadePosition;
            }
        }
    };

    class LineLaunch : public Pattern<RGBA>
    {
        TriggerFade<> fade;
        PixelMap *map;
        bool lastActive;
        int velocity;

    public:
        LineLaunch(PixelMap *map, int velocity = 100, int length = 20)
        {
            this->map = map;
            this->name = "Line launch";
            this->fade.setDuration(length);
            this->velocity = velocity;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!fade.isActive(active, 2*velocity))
                return;

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue((1 + map->y(i)) * velocity);
                RGBA color = params->getSecondaryColor();
                pixels[i] = color * fadePosition;
            }
        }
    };


    class Meteor : public Pattern<RGBA>
    {
        TriggerFade<> fade;
        int minDuration, maxDuration;
        int meteorCount;
        Permute perm;
        std::vector<int> durations;

    public:
        Meteor(int meteorCount = 5, int minDuration = 100, int maxDuration = 500, const char* name = "Meteor")
        {            
            this->meteorCount = meteorCount;
            this->minDuration = minDuration;
            this->maxDuration = maxDuration;
            this->name = name;
            perm.setSize(meteorCount);
            for (int i = 0; i < meteorCount; i++)
                durations.push_back(Utils::random(minDuration, maxDuration));
        }  

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            const int meteorSize = width / meteorCount;

            if (!fade.isActive(active, maxDuration, 200))
                return;

            if (active && fade.getPhase() == 0)
            {
                durations.clear();
                for (int i = 0; i < meteorCount; i++)
                    durations.push_back(Utils::random(minDuration, maxDuration));
                perm.permute();
            }

            for (int i = 0; i < width; i++)
            {
                int meteorIndex = i / meteorSize;
                int meteorPosition = i % meteorSize;

                // how fast it travels
                float fadeDelay = durations[meteorIndex] * meteorPosition / float(meteorSize);
                // length of the tail
                float fadeDuration = params->getSize(100, 10) ;

                float fadePosition = fade.getValue(fadeDelay, fadeDuration);
                pixels[i] = params->getGradientf(fadePosition) * fadePosition;
            }
        }
    };


class FadeFromRandom : public Pattern<RGBA>
    {
        int segmentSize;
        TriggerFade<> fade;
        BeatWatcher watcher;
        int centers[48];
        int delays[48];
        float durations[48];
        TriggerFade<> masterFade;

    public:
        FadeFromRandom(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Fade from random";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            fade.isActive(active); // this check alse retriggers the fade, so even though we dont need the return value here, we still need to call it.
            if (!masterFade.isActive(active))
                return;

            if (active && masterFade.getPhase() == 0)
            {
                for (int i = 0; i < 48; i++)
                {
                    centers[i] = Utils::random(segmentSize * 0.25, segmentSize * 0.75);
                    delays[i] = Utils::random(0, 500);
                    durations[i] = Utils::random_f()*2+0.5;
                }
            }

            // masterFade.setDuration(params->getSize(3000, 500));

            float trailSize = params->getVelocity(15, 5);
            fade.setDuration(params->getSize(1200, 300));
            float offset = params->getOffset();

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int distance = abs(centers[bar] - i);
                    float fadeValue = fade.getValue(distance * trailSize * durations[bar] + delays[bar] * offset);
                    pixels[bar * segmentSize + i] += params->getPrimaryColor() * fadeValue * masterFade.getValue();
                }
            }
        }
    };



    class GlitterFade : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        LFO<Glow> lfo;
        TriggerFade<> fade;
        Permute perm;

    public:
        GlitterFade(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Glitter fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            float velocity = params->getVelocity(800, 200);

            if (!fade.isActive(active, velocity))
                return;

            perm.setSize(width);

            lfo.setDutyCycle(params->getAmount(0.1,0.5));
            fade.setDuration(params->getSize(700, 200));


            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float glow = lfo.getValue(float(perm.at[index]) / width);
                float fade = this->fade.getValue(map->r(index) * velocity);
                pixels[index] = params->getPrimaryColor() * glow * fade;
            }
        }
    };

}