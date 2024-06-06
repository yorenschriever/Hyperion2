#pragma once
#include "colours.h"
#include "generation/controlHub/paletteColumn.hpp"
#include "generation/gradient.hpp"

// https://webgradients.com/
// https://color.adobe.com/create/color-wheel

Palette tunnel{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0x03A688)},
        {.position = 63, .color = RGB(0x026873)},
        {.position = 127, .color = RGB(0x011F26)},
        {.position = 255, .color = RGB(0x025E73)},

    }),
    .primary = RGB(0x03A688),
    .secondary = RGB(0xF2668B),
    .highlight = RGB(255, 255, 255),
    .name = "Tunnel"};
