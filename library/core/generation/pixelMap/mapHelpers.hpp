#include "pixelMap.hpp"
#include "core/distribution/utils/indexMap.hpp"
#include <math.h>
#include <cfloat>
#include <memory>

PixelMapPtr circleMap(int amount, float radius, float center_x=0, float center_y=0)
{
    PixelMap map;
    for (int i=0; i<amount; i++)
    {
        map.push_back({
            .x = float(center_x + radius * cos(float(i) / amount * 2 * M_PI)),
            .y = float(center_y + radius * sin(float(i) / amount * 2 * M_PI))
        });
    }
    return std::make_shared<PixelMap>(map);
}

PixelMapPtr gridMap(int amount_hor, int amount_ver, float distance_x=0, float distance_y=0, float center_x=0, float center_y=0)
{
    PixelMap map;

    if (distance_x==0){
        //calculate distance by filling the entire canvas
        distance_x = 2. / std::max(amount_hor, amount_ver);
    }
    if (distance_y==0){
        //calculate distance by filling the entire canvas
        distance_y = 2. / std::max(amount_hor, amount_ver);
    }

    float start_x  = center_x - ((amount_hor-1) * distance_x )/2;
    float start_y  = center_y - ((amount_ver-1) * distance_y )/2;

    for (int y=0; y<amount_ver; y++)
    {
        for (int x=0; x<amount_hor; x++)
        {
            map.push_back({
                .x = start_x + x * distance_x,
                .y = start_y + y * distance_y
            });
        }
    }
    return std::make_shared<PixelMap>(map);
}

PixelMapPtr combineMaps(std::vector<PixelMapPtr> maps)
{
    PixelMap result;
    for (auto map : maps)
        result.insert(result.end(), map->begin(), map->end());
    return std::make_shared<PixelMap>(result);
}

PixelMap3dPtr circleMap3d(int amount, float radius, float center_x=0, float center_y=0, float center_z=0)
{
    PixelMap3d map;
    for (int i=0; i<amount; i++)
    {
        map.push_back({
            .x = float(center_x + radius * cos(float(i) / amount * 2 * M_PI)),
            .y = float(center_y + radius * sin(float(i) / amount * 2 * M_PI)),
            .z = center_z
        });
    }
    return std::make_shared<PixelMap3d>(map);
}