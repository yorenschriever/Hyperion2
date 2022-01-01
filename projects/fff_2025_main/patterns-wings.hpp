#pragma once
#include <math.h>
#include <vector>

namespace Wings
{

    class XY : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        PixelMap *map;
        LFO<Sin> lfoX = LFO<Sin>(2000);
        LFO<Sin> lfoZ = LFO<Sin>(2000);

    public:
        XY(PixelMap *map)
        {
            this->name = "XY";
            this->map = map;
        }

        inline float softEdge(float dist, float size)
        {
            float edgeSize = 0.03;
            if (dist > size)
                return 0;

            if (dist < size - edgeSize)
                return 1;

            return (size - dist) / edgeSize;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            auto col = params->getSecondaryColour() * transition.getValue();
            float size = params->getSize(0.02, 0.1);
            lfoX.setPeriod(params->getVelocity(4 * 20000, 2000));
            lfoZ.setPeriod(params->getVelocity(4 * 14000, 1400));
            int variant = params->getVariant(0, 3);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float x_norm = Utils::rescale(map->x(index), 0, 1, -1.2, 1.2);
                float z_norm = Utils::rescale(map->y(index), 0, 1, 0, 1);

                if (variant == 1)
                    x_norm = 99;
                if (variant == 2)
                    z_norm = 99;

                float dim = std::max(
                    softEdge(abs(x_norm - lfoX.getValue()), size),
                    softEdge(abs(z_norm - lfoZ.getValue()), size));

                pixels[index] = col * dim;
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
        BeatWatcher watcher;
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
            if (this->watcher.Triggered(beatDiv))
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

}