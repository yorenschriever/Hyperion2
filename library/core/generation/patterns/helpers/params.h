#pragma once
#include "colors.h"
#include "core/generation/gradient.hpp"
#include "core/generation/palette.hpp"
#include <string>

Palette defaultPalette{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0, 0, 0)},        // Black
        {.position = 128, .color = RGB(255, 0, 0)},    // Red
        {.position = 224, .color = RGB(255, 255, 0)},  // Bright yellow
        {.position = 255, .color = RGB(255, 255, 255)} // Full white
    }),
    .primary = RGB(255, 0, 0),
    .secondary = RGB(0, 255, 0),
    .highlight = RGB(255, 255, 255),
    .name=""};


class Params
{
public:
    Params() {}
    Params(std::string name)
    {
        this->name = name;
    }
    std::string name;

    RGBA getPrimaryColor() { return palette->primary; }
    RGBA getSecondaryColor() { return palette->secondary; }
    RGBA getHighlightColor() { return palette->highlight; }
    RGBA getGradient(int pos) { return palette->gradient.get(pos); }
    RGBA getGradientf(float pos) { return palette->gradient.getf(pos); }

    float getVelocity(float start = 0, float end = 1) { return convert(start, end, velocity); }
    float getAmount(float start = 0, float end = 1) { return convert(start, end, amount); }
    float getIntensity(float start = 0, float end = 1) { return convert(start, end, intensity); }
    float getVariant(float start = 0, float end = 1) { return convert(start, end, variant); }
    float getSize(float start = 0, float end = 1) { return convert(start, end, size); }
    float getOffset(float start = 0, float end = 1) { return convert(start, end, offset); }

    Palette *palette = &defaultPalette;

    float velocity = 0.5;
    float amount = 0.5;
    float intensity = 0.5;
    float variant = 0.5;
    float size = 0.5;
    float offset = 0.5;

private:
    float convert(float start, float end, float value)
    {
        float distance = end - start;
        return start + value * distance;
    }
};

