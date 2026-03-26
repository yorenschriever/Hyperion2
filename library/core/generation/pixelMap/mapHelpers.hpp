#include "pixelMap.hpp"
#include <math.h>
#include <cfloat>

PixelMap circleMap(int amount, float radius, float center_x=0, float center_y=0)
{
    PixelMap map;
    for (int i=0; i<amount; i++)
    {
        map.push_back({
            .x = float(center_x + radius * cos(float(i) / amount * 2 * M_PI)),
            .y = float(center_y + radius * sin(float(i) / amount * 2 * M_PI))
        });
    }
    // map.push_back({0,0});
    return map;
}

PixelMap gridMap(int amount_hor, int amount_ver, float distance_x=0, float distance_y=0, float center_x=0, float center_y=0)
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
    return map;
}

PixelMap combineMaps(std::vector<PixelMap> maps)
{
    PixelMap result;
    for (auto map : maps)
        result.insert(result.end(), map.begin(), map.end());
    return result;
}

PixelMap resizeAndTranslateMap(PixelMap map, float scale, float x=0, float y=0)
{
    PixelMap result;
    for (auto pos : map)
        result.push_back({
            pos.x * scale + x,
            pos.y * scale + y
        });
    return result;
}

PixelMap rotateMap(PixelMap map, float angle)
{
    PixelMap result;
    angle = angle / 180 * M_PI;
    for (auto pos : map)
    {
        float x = pos.x * cos(angle) - pos.y * sin(angle);
        float y = pos.x * sin(angle) + pos.y * cos(angle);
        result.push_back({x, y});
    }
    return result;
}

PixelMap panelizeMap(PixelMap map, int panels_x, int panels_y, int pos_x, int pos_y)
{
    float scale = 1. / std::max(panels_x, panels_y);
    // float start_x  = (((panels_x-1) + pos_x) * scale );
    // float start_y  = (((panels_y-1) + pos_y) * scale );
    // float startx = (pos_x - (panels_x-1)/2.) * scale;
    // float starty = (pos_y - (panels_y-1)/2.) * scale;

    float start_x = -1 + scale + pos_x * scale*2;
    float start_y = -1 + scale + pos_y * scale*2;

    return resizeAndTranslateMap(
        map,
        scale,
        start_x,
        start_y
    );
}

/*
This rescales a map to fit in a -1 to 1 box. By default it deforms the map.
This is useful for patterns that work on best a -1 to 1 coordinate system, and your map is smaller (or bigger).
*/
PixelMap normalizeMap(PixelMap map, bool keepAspectRatio = false)
{
    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = -FLT_MAX;
    float max_y = -FLT_MAX;

    for (auto pos : map)
    {
        if (pos.x < min_x)
            min_x = pos.x;
        if (pos.y < min_y)
            min_y = pos.y;
        if (pos.x > max_x)
            max_x = pos.x;
        if (pos.y > max_y)
            max_y = pos.y;
    }

    float scale_x;
    float scale_y;
    float x;
    float y;

    if (keepAspectRatio)
    {
        float scale = 2. / std::max(max_x - min_x, max_y - min_y);
        scale_x = scale;
        scale_y = scale;

        x = -1 - min_x * scale;
        y = -1 - min_y * scale;
    }
    else
    {
        scale_x = 2. / (max_x - min_x);
        scale_y = 2. / (max_y - min_y);

        x = -1 - min_x * scale_x;
        y = -1 - min_y * scale_y;
    }

    PixelMap result;
    for (auto pos : map)
        result.push_back({
            pos.x  * scale_x + x,
            pos.y  * scale_y + y
        });
    return result;
}