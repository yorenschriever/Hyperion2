
#pragma once
#include "hyperion.hpp"
// #include "mappingHelpers.hpp"
// #include "../mapping/schwungMap.hpp"
#include <math.h>
#include <vector>


namespace SchwungPatterns
{

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
            PixelMap::Polar *map;
        FadeDown fade1 = FadeDown(200);
        FadeDown fade2 = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();
        Permute perm1;
        Permute perm2;
        int fadeNr =0;

    public:
        RadialGlitterFadePattern(PixelMap::Polar *map)
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
            // float fromCenter = params->getVariant() >= 0.5;

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

                float conePos = map->r(i);

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
        PixelMap::Polar *map;
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
        AngularFadePattern(PixelMap::Polar *map)
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
        PixelMap::Polar *map;
        FadeDown fade = FadeDown(2000);

    public:
        GrowingStrobePattern(PixelMap::Polar *map)
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
                float conePos = 0.20 + map->r(i)/1.2;
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
        PixelMap::Polar *map;
        FadeDown fade = FadeDown(200);
        BeatWatcher watcher = BeatWatcher();

    public:
        RadialFadePattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Radial fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            fade.duration = params->getSize(500, 120);
            int velocity = params->getVelocity(1000, 50);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
            }

            for (int i = 0; i < map->size(); i++)
            {
                float conePos = 0.20 + map->r(i) ;
                pixels[i] += params->getPrimaryColour() * fade.getValue(conePos* velocity) * transition.getValue();
            }
        }
    };


    class StaticGradientPattern : public Pattern<RGBA>
    {
        PixelMap::Polar *map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        StaticGradientPattern(PixelMap::Polar *map)
        {
            this->map = map;
            this->name = "Static gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            // float height = params->getSize(0.2,1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float h = map->r(index); // fromBottom(map->y(index))* height;
                RGBA colour = params->getGradient(h * 255);
                pixels[index] = colour  * transition.getValue();
            }
        }
    };

    class FadeFromCenter : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        static const int numFades = 3;
        int currentFade = 0;
        FadeDown fade[numFades];
        BeatWatcher wachter;
        int beatDivs[5] = {16, 8, 4, 2, 2};

    public:
        FadeFromCenter(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Fade from center";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            int beatDiv = beatDivs[int(params->getAmount(0, 4))];
            if (this->wachter.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
            {
                currentFade = (currentFade + 1) % numFades;
                fade[currentFade].reset();
            }

            if (!transition.Calculate(active))
                return;

            float trailSize = params->getSize(1, 10);
            float velocity = params->getVelocity(250, 5);

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int center = segmentSize / 2;
                    int distance = abs(center - i);
                    for (int f = 0; f < numFades; f++)
                    {
                        float fadeValue = fade[f].getValue(distance * trailSize + (bar % (width / segmentSize / 4)) * velocity);
                        pixels[bar * segmentSize + i] += params->getGradient(255 * fadeValue) * fadeValue * transition.getValue();
                    }
                }
            }
        }
    };

    class FadeFromRandom : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade;
        BeatWatcher watcher;
        int beatDivs[6] = {16, 8, 4, 2, 1, 1};
        int centers[48];
        int delays[48];
        FadeDown masterFade;

    public:
        FadeFromRandom(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Fade from random";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            int beatDiv = beatDivs[int(params->getAmount(0, 5))];
            if (this->watcher.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
            {
                fade.reset();
                masterFade.reset();
                for (int i = 0; i < 48; i++)
                {
                    centers[i] = Utils::random(15, 45);
                    delays[i] = Utils::random(0, 500);
                }
            }

            if (!transition.Calculate(active))
                return;

            masterFade.duration = Tempo::TimeBetweenBeats() * beatDiv;

            float trailSize = params->getVelocity(15, 5);
            fade.setDuration(params->getSize(1200, 300));
            float offset = params->getOffset();

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int distance = abs(centers[bar] - i);
                    float fadeValue = fade.getValue(distance * trailSize + delays[bar] * offset);
                    pixels[bar * segmentSize + i] += params->getPrimaryColour() * fadeValue * masterFade.getValue() * transition.getValue();
                }
            }
        }
    };

    class FadeStarPattern : public Pattern<RGBA>
    {
        int segmentSize;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade;
        BeatWatcher watcher;
        int beatDivs[6] = {16, 8, 4, 2, 1, 1};
        int centers[48];
        int delays[48];
        FadeDown masterFade;

    public:
        FadeStarPattern(int segmentSize = 60)
        {
            this->segmentSize = segmentSize;
            this->name = "Fade star";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            int beatDiv = beatDivs[int(params->getAmount(0, 5))];
            if (this->watcher.Triggered() && Tempo::GetBeatNumber() % beatDiv == 0)
            {
                fade.reset();
                masterFade.reset();
                for (int i = 0; i < 48; i++)
                {
                    centers[i] = 30; //Utils::random(15, 45);
                    delays[i] = Utils::random(0, 500);
                }
            }

            if (!transition.Calculate(active))
                return;

            masterFade.duration = Tempo::TimeBetweenBeats() * beatDiv;

            float trailSize = params->getVelocity(15, 5);
            fade.setDuration(params->getSize(1200, 300));
            float offset = params->getOffset();

            for (int bar = 0; bar < width / segmentSize; bar++)
            {
                for (int i = 0; i < segmentSize; i++)
                {
                    int distance = (segmentSize/2)-abs(centers[bar] - i);
                    float fadeValue = fade.getValue(distance * trailSize + delays[bar] * offset);
                    pixels[bar * segmentSize + i] += params->getPrimaryColour() * fadeValue * masterFade.getValue() * transition.getValue();
                }
            }
        }
    };


    class HexagonStrobePattern : public Pattern<RGBA>
    {
        int hex =0;
        public:
        HexagonStrobePattern()
        {
            this->name = "Hexagon strobe";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            hex = (hex + Utils::random(0, 11)) % 12;

            for (int i = 0; i < 6*60; i++)
            {
                pixels[hexagons[hex][i]] = params->getSecondaryColour();
            }
        }
    };

    class OnBeatHexagonFade : public Pattern<RGBA>
    {
        int hex =0;
        Transition transition; 
        BeatWatcher watcher;
        FadeDown fade;

        public:
        OnBeatHexagonFade()
        {
            this->name = "Onbeat Hex Fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            fade.setDuration(params->getVelocity(1000, 100));

            if (watcher.Triggered()){
                hex = (hex + Utils::random(0, 11)) % 12;
                fade.reset();
            }

            for (int i = 0; i < 6*60; i++)
            {
                pixels[hexagons[hex][i]] = params->getSecondaryColour() * fade.getValue() * transition.getValue();
            }
        }
    };


    class HexChaser : public Pattern<RGBA>
    {
        Transition transition = Transition(
            400, Transition::none, 0,
            400, Transition::none, 0);

        LFO<SawDown> lfo;
    public:
        HexChaser()
        {
            this->name = "HexChaser 2";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            lfo.setDutyCycle(params->getAmount(0.1, 0.5));
            lfo.setPeriod(params->getVelocity(10000, 500));
            float offset = params->getOffset();

            int hexi=0;
            for(auto hex: hexagons)
            {
                for (int i = 0; i < 6*60; i++)
                {
                    int index = hex[i];
                    float lfoValue = lfo.getValue(float(i)/(6*60) + offset * hexi);
                    RGBA value = params->getGradient(lfoValue * 255) * lfoValue * transition.getValue();
                    pixels[index] += value;
                }
                hexi++;
            }

            // auto hex = hexagons[3];
            // for (int i = 0; i < 6 * 60; i++)
            // {
            //     int index = hex[i];
            //     RGBA value = RGBA(255, 0, 0, 255) * lfo.getValue(float(i)/(6*60)) * transition.getValue();
            //     pixels[index] = value;
            // }
        }
    };
}