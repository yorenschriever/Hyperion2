#pragma once

#include <inttypes.h>
#include <vector>
#include <math.h>

struct PixelPosition
{
    float x;
    float y;
};

typedef std::vector<PixelPosition> PixelMap;

struct PolarPixelPosition
{
    float r;
    float th;
};

typedef std::vector<PolarPixelPosition> PolarPixelMap;

PolarPixelMap toPolar(PixelMap map)
{
    PolarPixelMap result;
    std::transform(
        map.begin(), 
        map.end(), 
        std::back_inserter(result), [](PixelPosition pos) -> PolarPixelPosition{ 
            return {
                .r = sqrt(pos.y * pos.y + pos.x * pos.x),
                .th = atan2(pos.y, pos.x)
            };
        });
    return result;
}