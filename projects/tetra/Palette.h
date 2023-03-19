#pragma once

#include "colours.h"
#include <stdint.h>

#define LINEARBLEND 0

// typedef struct
// {
//     uint8_t position;
//     RGBA color;
// } PaletteEntry;

// typedef std::vector<PaletteEntry> Palette;

// RGBA ColorFromPalette(const Palette palette, uint8_t index, uint8_t brightness, void*)
// {
//     auto entry = palette[index].color;
//     entry.dim(brightness);
//     return entry;
// }

// uint8_t scale8_video_LEAVING_R1_DIRTY( uint8_t i, uint8_t scale)
// {
//     uint8_t j = (((int)i * (int)scale) >> 8) + ((i&&scale)?1:0);
//     return j;
// }

// RGBA ColorFromPalette( const Palette pal, uint8_t index, uint8_t brightness, void*)
// {
//     //const CRGB* entry = &(pal[0]) + index;
//     auto entry = pal[index].color;

//     uint8_t red   = entry.R;
//     uint8_t green = entry.G;
//     uint8_t blue  = entry.B;

//     if( brightness != 255) {
//         ++brightness; // adjust for rounding
//         red   = scale8_video_LEAVING_R1_DIRTY( red,   brightness);
//         green = scale8_video_LEAVING_R1_DIRTY( green, brightness);
//         blue  = scale8_video_LEAVING_R1_DIRTY( blue,  brightness);
//         //cleanup_R1();
//     }

//     return RGBA( red, green, blue, 255);
// }


class Palette
{
public:
    typedef struct
    {
        uint8_t position;
        RGBA color;
    } PaletteEntry;

    Palette(std::vector<PaletteEntry> entries)
    {
        int index = 0;
        RGBA start;

        for (auto entry : entries)
        {
            for (int j = index; j <= entry.position; j++)
            {
                int fade_position = 256 * (j - index) / (j - entry.position + 1);
                map[j] = start + entry.color * float(fade_position) / 255;
            }
            start = entry.color;
            index = entry.position;
        }
    }

    RGBA get(uint8_t position)
    {
        return map[position];
    }

private:
    RGBA map[255];
};

RGBA ColorFromPalette( Palette *palette, uint8_t index, uint8_t brightness, void*)
{
    auto entry = palette->get(index);
    //entry.dim(brightness);
    return entry;
}
