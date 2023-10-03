#pragma once
#include <stdint.h>
#include <vector>

struct Animation
{
    int frames;
    int pixels;
    int depth;
    std::vector<uint8_t> pixelData;
};