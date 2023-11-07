#pragma once
#include "colours.h"
#include "core/generation/gradient.hpp"
#include "core/generation/palette.hpp"

// https://webgradients.com/
// https://color.adobe.com/create/color-wheel
Palette grandMA {
    .gradient = Gradient({
        {.position = 0, .color = RGB(0, 0, 0)},        // Black
        {.position = 128, .color = RGB(255, 0, 0)},    // Red
        {.position = 224, .color = RGB(255, 255, 0)},  // Bright yellow
        {.position = 255, .color = RGB(255, 255, 255)} // Full white
    }),
    .primary = RGB(255, 0, 0),
    .secondary = RGB(255, 255, 0),
    .highlight = RGB(255, 255, 255),
    .name = "GrandMA"};
