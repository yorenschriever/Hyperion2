
#pragma once
#include "mappingHelpers.hpp"
#include <math.h>
#include <vector>

namespace Max
{

    class ChevronsPattern : public Pattern<RGBA>
    {
        PixelMap3d *map;
        LFO<SawDown> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade1 = FadeDown(1400);

    public:
        ChevronsPattern(PixelMap3d *map)
        {
            this->map = map;
            this->lfo = LFO<SawDown>(1000);
            this->lfoColour = LFO<Square>(1000);
            this->name = "Chevrons";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float amount = params->getAmount(0.25, 4);
            lfo.setPeriod(params->getVelocity(2000, 500));
            lfoColour.setPeriod(params->getOffset(2000, 500));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float dir = (map->z(index) > 0 || map->y(index) > 0.44) ? 1:-1;
                float phase = (0.5 * abs(map->x(index)) - map->z(index) + map->y(index) * dir) * amount;
                auto col = lfoColour.getValue(phase) ? params->getSecondaryColour() : params->getPrimaryColour();
                pixels[index] += col * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class ChevronsConePattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        LFO<SawDown> lfo;
        LFO<Square> lfoColour;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        ChevronsConePattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Chevrons cone";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            float amount = params->getAmount(0.25, 4);
            lfo.setPeriod(params->getVelocity(2000, 500));
            lfoColour.setPeriod(params->getOffset(2000, 500));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float conePos = 0.5 + (map->r(index) - map->z(index)) / 2;

                float phase = conePos * amount; //(0.5 * abs(map->y(index)) + map->x(index)) * amount;
                auto col = (lfoColour.getValue(phase) > 0) ? params->getSecondaryColour() : params->getPrimaryColour();
                pixels[index] += col * lfo.getValue(phase) * transition.getValue();
            }
        }
    };

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;
        FadeDown fade1 = FadeDown(200);
        FadeDown fade2 = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();
        Permute perm1;
        Permute perm2;
        int fadeNr =0;

    public:
        RadialGlitterFadePattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->perm1 = Permute(map->size());
            this->perm2 = Permute(map->size());
            this->name = "Radial glitter fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade1.duration = params->getIntensity(500, 100);
            fade2.duration = params->getIntensity(500, 100);

            // timeline.FrameStart();
            // if (timeline.Happened(0))
            if (watcher.Triggered())
            {
                fadeNr = (fadeNr+1)%2;
                if (fadeNr==0){
                    fade1.reset();
                    perm1.permute();
                } else {
                    fade2.reset();
                    perm2.permute();
                }
            }

            float velocity = params->getVelocity(600, 100);
            // float trail = params->getIntensity(0,1) * density;
            // float trail = params->getIntensity(0, 200);
            float fromCenter = params->getVariant() >= 0.5;

            if (perm1.at ==0 || perm2.at==0)
                return;

            for (int i = 0; i < map->size(); i++)
            {
                // fade.duration = perm.at[i] * trail; // + 100;
                // fade.duration = (perm.at[i] * trail) < map->size() / 20 ? ; // + 100;

                // fade.duration = 100;
                // if (perm.at[i] < density * map->size()/ 10)
                //     fade.duration *= 4;

                float density = 481. / width;
                fade1.duration = 100; // trail + perm.at[i] / (density * map->size()/ 10);
                fade2.duration = 100; 
                if (perm1.at[i] < density * map->size() / 10)
                    fade1.duration *= perm1.at[i] * 4 / (density * map->size() / 10);
                if (perm2.at[i] < density * map->size() / 10)
                    fade2.duration *= perm2.at[i] * 4 / (density * map->size() / 10);

                float conePos = fromCenter ? 
                    // vanaf midden
                    (0.5 + (map->r(i) - map->z(i)) / 2) :
                    // vanaf de knik beide kanten op
                    (1-(map->r(i) + map->z(i))/2);

                float fadePosition1 = fade1.getValue(conePos * velocity);
                RGBA color1 = params->getGradient(fadePosition1 * 255);
                pixels[i] = color1 * fadePosition1 * (1.5 - map->r(i)) * transition.getValue();

                float fadePosition2 = fade2.getValue(conePos * velocity);
                RGBA color2 = params->getGradient(fadePosition2 * 255);
                pixels[i] += color2 * fadePosition2 * (1.5 - map->r(i)) * transition.getValue();
            }
        }
    };

    class AngularFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;
        static const int numFades = 4;
        int fadeNr=0;
        FadeDown fade[numFades] = {
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200)
        };
        BeatWatcher watcher = BeatWatcher();

    public:
        AngularFadePattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Angular fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            int beatDiv = params->getVariant(0,4);
            if (beatDiv >3) beatDiv =3;
            int divs[] = {8,4,2,1};

            if (watcher.Triggered() && Tempo::GetBeatNumber() % divs[beatDiv]==0)
            {
                fadeNr = (fadeNr+1)%numFades;
                fade[fadeNr].reset();
                // perm.permute();
            }

            // float density = 481./width;
            float velocity = params->getVelocity(1000, 30);
            float trail = params->getSize(40, 200);

            for(int f=0;f<numFades; f++){
                fade[f].duration = trail;
            }

            for (int i = 0; i < map->size(); i++)
            {
                //fade.duration = 80; // trail + perm.at[i] / (density * map->size()/ 10);
                // if (perm.at[i] < density * map->size()/ 10)
                //     fade.duration *= perm.at[i] * 4 / (density * map->size()/ 10);
                for(int f=0;f<numFades; f++){
                    float fadePosition = fade[f].getValue(abs(map->th(i)) * velocity);
                    RGBA color = params->getGradient(255 - abs(map->th(i)) / M_PI * 255);
                    pixels[i] += color * fadePosition * (map->r(i) * 1.5) * transition.getValue();
                }
            }
        }
    };

    class GrowingStrobePattern : public Pattern<RGBA>
    {
        PixelMap3d::Cylindrical *map;
        FadeDown fade = FadeDown(2000);

    public:
        GrowingStrobePattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Growing strobe";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
            {
                fade.reset();
                return;
            }

            RGBA col = Utils::millis() % 100 < 25 ? params->getPrimaryColour() : RGBA();
            int directionUp = params->getVariant() > 0.5;

            for (int i = 0; i < map->size(); i++)
            {
                float conePos = 0.20 + (map->r(i) - map->z(i)) / 2;
                if (directionUp && conePos < fade.getValue())
                    continue;
                if (!directionUp && 1. - conePos < fade.getValue())
                    continue;

                pixels[i] = col;
            }
        }
    };

    class RadialFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        RadialFadePattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Radial fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            fade.duration = params->getSize(500, 120);
            int velocity = params->getVelocity(500, 50);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            for (int i = 0; i < map->size(); i++)
            {
                float conePos = 0.20 + (map->r(i) - map->z(i)) / 2;
                pixels[i] += params->getPrimaryColour() * fade.getValue(conePos* velocity) * transition.getValue();
            }
        }
    };

   
};