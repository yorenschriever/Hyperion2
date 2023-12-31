#pragma once
#include "video.hpp"
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>
class BgPattern : public Pattern<RGBA>
{

public:
    BgPattern()
    {
        this->name = "BG";
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!active)
            return;

        for (int i = 0; i < width; i++)
            pixels[i] = params->getSecondaryColour();
    }
};

uint8_t *readFile(const char *filename)
{
    streampos size;
    char *memblock;

    ifstream file(filename, ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];
        file.seekg(0, ios::beg);
        file.read(memblock, size);
        file.close();

        return (uint8_t*)memblock;
    };
    return nullptr;
}

Video readVideo(const char *filename)
{
    uint8_t * content = readFile(filename);
    Video result =  Video{0,0,0,0};
    if (!content){
        Log::error("VIDEO","Unable to open video file: %s", filename);
        return result;
    }

    result.frames = content[1] + (content[0]<<8);
    result.pixels = content[3] + (content[2]<<8);
    result.depth = content[4];
    result.pixelData = &content[5];

    Log::info("VIDEO","opened video file: %s, frames=%d, pixels=%d, depth=%d", filename, result.frames, result.pixels, result.depth);

    return result;
}


class VideoPattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    int frame = 0;
    Video video;

public:
    VideoPattern(const char *filename, const char *name = "Video")
    {
        this->name = name;
        this->video = readVideo(filename);
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        frame++;
        if (frame >= video.frames)
            frame = 0;

        for (int i = 0; i < width; i++)
        {
            int animationIndex = (frame * video.pixels + i) * video.depth;

            pixels[i] = (RGBA)RGB(
                            video.pixelData[animationIndex + 0],
                            video.pixelData[animationIndex + 1],
                            video.pixelData[animationIndex + 2]) *
                        transition.getValue();
            if (video.depth == 4)
                pixels[i] = pixels[i] * (float(video.pixelData[animationIndex + 3]) / 255.);
        }
    }
};

class VideoPalettePattern : public Pattern<RGBA>
{
    Transition transition = Transition(
        200, Transition::none, 0,
        1000, Transition::none, 0);
    int frame = 0;
    Video video;

public:
    VideoPalettePattern(const char *filename, const char *name = "Video")
    {
        this->name = name;
        this->video = readVideo(filename);
    }

    inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
    {
        if (!transition.Calculate(active))
            return;

        frame++;
        if (frame >= video.frames)
            frame = 0;

        for (int i = 0; i < width; i++)
        {
            int animationIndex = (frame * video.pixels + i) * video.depth;

            pixels[i] += params->getPrimaryColour() * (float(video.pixelData[animationIndex + 0]) / 255.);
            pixels[i] += params->getSecondaryColour() * (float(video.pixelData[animationIndex + 1]) / 255.);
            // pixels[i] += params->getHighlightColour() * (float(video.pixelData[animationIndex + 2]) / 255.);
            pixels[i] += params->getGradient(0) * (float(video.pixelData[animationIndex + 2]) / 255.);

            pixels[i] = pixels[i] * transition.getValue();
        }
    }
};