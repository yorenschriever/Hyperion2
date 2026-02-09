#pragma once

#include "colors.h"
#include <stdint.h>
#include <vector>
#include "utils.hpp"

class Gradient
{
public:
    typedef struct {
        uint8_t position;
        RGBA color;
    } GradientEntry;

    Gradient(std::vector<GradientEntry> entries)
    {
        int start_index = 0;
        RGBA start_color;

        for (auto &entry : entries)
        {
            for (int j = start_index; j < entry.position; j++)
            {
                float fade_position = float(j - start_index) / (entry.position - start_index + 1);
                map[j] = start_color + entry.color * fade_position;
            }
            map[entry.position] = entry.color;
            start_color = entry.color;
            start_index = entry.position;
        }
    }

    //position in range 0-255
    RGBA get(int position)
    {
        return map[Utils::constrain(position,0,255)];
    }

    //position in range 0-1
    RGBA getf(float position)
    {
        return map[Utils::constrain(position*255,0,255)];
    }

private:
    RGBA map[256];
};
