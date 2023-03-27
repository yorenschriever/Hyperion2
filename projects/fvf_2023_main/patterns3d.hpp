#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include "ledsterPatterns.hpp"
#include <math.h>
#include <vector>

namespace FWF3D
{

    class RadialGlitterFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        Permute perm;

    public:
        RadialGlitterFadePattern(PixelMap3d map)
        {
            this->map = map.toCylindricalRotate90();
            this->perm = Permute(map.size());
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            fade.duration = Params::getIntensity(500, 100);

            // timeline.FrameStart();
            // if (timeline.Happened(0))
            if (watcher.Triggered())
            {
                fade.reset();
                perm.permute();
            }

            float velocity = Params::getVelocity(600, 100);
            // float trail = Params::getIntensity(0,1) * density;
            // float trail = Params::getIntensity(0, 200);

            for (int i = 0; i < map.size(); i++)
            {
                // fade.duration = perm.at[i] * trail; // + 100;
                // fade.duration = (perm.at[i] * trail) < map.size() / 20 ? ; // + 100;

                // fade.duration = 100;
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= 4;

                float density = 481. / width;
                fade.duration = 100; // trail + perm.at[i] / (density * map.size()/ 10);
                if (perm.at[i] < density * map.size() / 10)
                    fade.duration *= perm.at[i] * 4 / (density * map.size() / 10);

                // vanaf de knik beide kanten op
                // float conePos = 1-(map[i].r + map[i].z)/2;
                // vanaf midden
                float conePos = 0.5 + (map[i].r - map[i].z) / 2;

                float fadePosition = fade.getValue(conePos * velocity);
                RGBA color = Params::palette->get(fadePosition * 255);
                pixels[i] = color * fadePosition * (1.5 - map[i].r) * transition.getValue();
            }
        }
    };

    class AngularFadePattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap3d::Cylindrical map;
        FadeDown fade = FadeDown(200, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();

    public:
        AngularFadePattern(PixelMap3d map)
        {
            this->map = map.toCylindricalRotate90();
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                fade.reset();
                // perm.permute();
            }

            // float density = 481./width;
            float velocity = Params::getVelocity(200, 30);
            // float trail = Params::getIntensity(0, 200);

            for (int i = 0; i < map.size(); i++)
            {
                fade.duration = 80; // trail + perm.at[i] / (density * map.size()/ 10);
                // if (perm.at[i] < density * map.size()/ 10)
                //     fade.duration *= perm.at[i] * 4 / (density * map.size()/ 10);

                float fadePosition = fade.getValue(abs(map[i].th) * velocity);
                RGBA color = Params::palette->get(255 - abs(map[i].th) / M_PI * 255);
                pixels[i] = color * fadePosition * (map[i].r * 1.5) * transition.getValue();
                ;
            }
        }
    };

    class OnBeatColumnChaseUpPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[6] = {
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd)};
        BeatWatcher watcher = BeatWatcher();
        PixelMap3d::Cylindrical map;
        Permute perm;
        int pos = 0;

    public:
        OnBeatColumnChaseUpPattern(PixelMap3d map)
        {
            this->map = map.toCylindricalRotate90();
            this->perm = Permute(map.size());
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            for (int i = 0; i < 6; i++)
                fade[i].duration = Params::getIntensity(3000, 100);

            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            float velocity = Params::getVelocity(600, 100);
            float density = 481. / width;

            for (int column = 0; column < 6; column++)
            {
                int columnStart = column * width / 6;
                int columnEnd = columnStart + width / 6;

                fade[column].duration = 150;

                for (int i = columnStart; i < columnEnd; i++)
                {
                    float conePos = 0.5 - (map[i].r - map[i].z) / 2;

                    float fadePosition = fade[column].getValue(conePos * velocity);
                    // RGBA color = Params::palette->get(fadePosition * 255);
                    RGBA color = Params::palette->get((1 - map[i].z) * 255);
                    pixels[i] = color * fadePosition * (1 - map[i].z) * transition.getValue();
                }
            }
        }
    };

    class GrowingCirclesPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade[6] = {
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd),
            FadeDown(200, WaitAtEnd)
        };
        std::vector<float> radii[6];
        BeatWatcher watcher = BeatWatcher();
        // PixelMap3d::Cylindrical map;
        Permute perm;
        int pos = 0;

    public:
        GrowingCirclesPattern(PixelMap3d map)
        {
            // this->map = map.toCylindricalRotate90();
            for (int i = 0; i < 6; i++)
            {
                float xc = cos(float(i)/6*2*M_PI);
                float yc = 0.4;
                float zc = sin(float(i)/6*2*M_PI);
                std::transform(map.begin(), map.end(), std::back_inserter(radii[i]), [xc,yc,zc](PixelPosition3d pos) -> float
                               { return sqrt(pow(pos.x-xc,2) + pow(pos.y-yc,2) + pow(pos.z-zc,2)); });
            }
            this->perm = Permute(map.size());
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered() && Tempo::GetBeatNumber() % 4==0)
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            float velocity = 200; //Params::getVelocity(600, 100);
            // float density = 481./width;

            for (int i = 0; i < width; i++)
            {
                for (int column = 0; column < 6; column++)
                {
                    float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                    RGBA color = Params::getPrimaryColour(); //::palette->get(fadePosition * 255);
                    pixels[i] += color * fadePosition; // * transition.getValue();
                }
            }
        }
    };

    class LineLaunch : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        FadeDown fade = FadeDown(50, WaitAtEnd);
        BeatWatcher watcher = BeatWatcher();
        PixelMap3d map;

    public:
        LineLaunch(PixelMap3d map)
        {
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active) override
        {
            if (!active && fade.getPhase() ==1)
                return;

            if (watcher.Triggered())
                fade.reset();

            float velocity = 100; //Params::getVelocity(600, 100);
            // float density = 481./width;

            for (int i = 0; i < width; i++)
            {
                float fadePosition = fade.getValue((1+map[i].z) * velocity);
                RGBA color = Params::getHighlightColour(); 
                pixels[i] += color * fadePosition; // * transition.getValue();
            }
        }
    };
};
