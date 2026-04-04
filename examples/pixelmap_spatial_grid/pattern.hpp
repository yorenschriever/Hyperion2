#pragma once

#include "core/generation/patterns/pattern.hpp"
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/lfo.h"
#include "core/generation/patterns/helpers/transition.h"
#include "platform/includes/utils.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

class BeerBubblesPattern : public Pattern<RGBA>
{
    static const int MAX_BUBBLES = 10000;

    struct Bubble
    {
        float x;
        float y;
        float speed;
        float size;
        float wobblePhase;
        float wobbleAmount;
        bool alive;
    };

    Bubble bubbles[MAX_BUBBLES];
    PixelMap *map;
    Transition transition = Transition(500, 1500);
    Timeline time;
    SpatialGrid grid;
    std::vector<int> candidates;

    void spawnBubble(int index)
    {
        bubbles[index].x = Utils::random_f() * 2.f - 1.f;
        bubbles[index].y = 1.2f;
        bubbles[index].speed = Utils::random_f() * 0.3f + 0.2f;
        bubbles[index].size = Utils::random_f() * 0.08f + 0.03f;
        bubbles[index].wobblePhase = Utils::random_f() * 6.28f;
        bubbles[index].wobbleAmount = Utils::random_f() * 0.05f + 0.02f;
        bubbles[index].alive = true;
    }

public:
    BeerBubblesPattern(PixelMap *map)
    {
        this->name = "Beer bubbles";
        this->map = map;
        grid.build(map);
        for (int i = 0; i < MAX_BUBBLES; i++)
            bubbles[i].alive = false;
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        time.FrameStart();

        float velocity = params->getVelocity(0.3f, 2.0f);
        int amount = params->getAmount(2, MAX_BUBBLES);
        float bubbleSize = params->getSize(1.5f, 0.3f);
        float timeStep = time.GetDelta() / 1000.0f;

        int activeBubbles = 
            std::count_if(std::begin(bubbles), std::end(bubbles), [](const Bubble &b)
                         { return b.alive; });
        int toSpawn = amount - activeBubbles;
        for (int i = 0; i < MAX_BUBBLES && toSpawn > 0; i++)
        {
            if (!bubbles[i].alive)
            {
                spawnBubble(i);
                toSpawn--;
            }
        }

        for (int i = 0; i < MAX_BUBBLES; i++)
        {
            if (!bubbles[i].alive) continue;
            bubbles[i].y -= bubbles[i].speed * velocity * timeStep;
            bubbles[i].wobblePhase += timeStep * 4.0f;
            if (bubbles[i].y < -1.3f)
                bubbles[i].alive = false;
        }

        float transVal = transition.getValue();

        for (int b = 0; b < MAX_BUBBLES; b++)
        {
            if (!bubbles[b].alive) continue;

            float bx = bubbles[b].x + sinf(bubbles[b].wobblePhase) * bubbles[b].wobbleAmount;
            float by = bubbles[b].y;
            float radius = bubbles[b].size * bubbleSize;

            float gradientVal = Utils::rescale_c(by, 0.0f, 1.0f, -1.0f, 1.0f);
            RGBA color = params->getGradientf(gradientVal);

            grid.getPixelsInRange(bx, by, radius, candidates);

            for (int p : candidates)
            {
                float dx = map->x(p) - bx;
                float dy = map->y(p) - by;
                float distSq = dx * dx + dy * dy;
                float radiusSq = radius * radius;

                if (distSq < radiusSq)
                {
                    float dist = sqrtf(distSq);
                    float brightness = 1.0f - (dist / radius);
                    brightness = brightness * brightness;
                    pixels[p] += color * brightness * transVal;
                }
            }
        }
    }
};
