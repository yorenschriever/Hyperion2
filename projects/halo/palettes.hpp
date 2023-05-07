#pragma once
#include "colours.h"
#include "generation/controlHub/paletteColumn.hpp"
#include "gradient.hpp"

// https://webgradients.com/
// https://color.adobe.com/create/color-wheel

// Each palette consists of:
// - A gradient: A smooth transition between colors.
// - A primary color: The most prominent color in the gradient
// - A secondary color: A contrasting color
// - A Highlight color: A bright color, contrasting 

PaletteColumn::Palette heatmap{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0, 0, 0)},        // Black
        {.position = 128, .color = RGB(255, 0, 0)},    // Red
        {.position = 224, .color = RGB(255, 255, 0)},  // Bright yellow
        {.position = 255, .color = RGB(255, 255, 255)} // Full white
    }),
    .primary = RGB(255, 0, 0),
    .secondary = RGB(255, 255, 0),
    .highlight = RGB(255, 255, 255),
    .name = "Heatmap"};

PaletteColumn::Palette sunset1{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(8, 7, 72)},        // Dark blue
        {.position = 120, .color = RGB(78, 150, 212)},  // Light blue
        {.position = 160, .color = RGB(78, 150, 212)},  // Light blue
        {.position = 200, .color = RGB(233, 200, 246)}, // White
        {.position = 230, .color = RGB(205, 0, 226)},   // Purple
        {.position = 255, .color = RGB(205, 0, 255)}    // Purple
    }),
    .primary = RGB(78, 150, 212),
    .secondary = RGB(205, 0, 226),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 1"};
