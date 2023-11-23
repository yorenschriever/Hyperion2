#pragma once
#include <stdint.h>
#include <vector>

struct Video
{
    int frames;
    int pixels;
    int depth;
    uint8_t* pixelData;
};