#pragma once
#include "colours.h"
#include "generation/controlHub/paletteColumn.hpp"
#include "gradient.hpp"

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
    .name = "sunset1"};

    PaletteColumn::Palette sunset3{
    .gradient = new Gradient({
    {.position = 0, .color = RGB(10, 20, 70)},      // Dark Blue
    {.position = 90, .color = RGB(32, 80, 118)},    // Lighter Blue
    {.position = 200, .color = RGB(170, 167, 153)}, // Grey
    {.position = 255, .color = RGB(200, 180, 50)}   // Greyish yellow
    }),
    .primary = RGB(32, 80, 118),
    .secondary = RGB(200, 180, 50),
    .highlight = RGB(255, 255, 255),
    .name = "sunset3"};

    PaletteColumn::Palette sunset4{
    .gradient = new Gradient({
    {.position = 0, .color = RGB(60, 130, 197)},    // Light blue
    {.position = 160, .color = RGB(200, 220, 240)}, // White blue
    {.position = 210, .color = RGB(243, 170, 50)},  // Light orange
    {.position = 255, .color = RGB(100, 40, 0)}     // Dark orange
    }),
    .primary = RGB(100, 40, 0),
    .secondary = RGB(60, 130, 197),
    .highlight = RGB(255, 255, 255),
    .name = "sunset4"};

// Some sunset color approximated from https://digitalsynopsis.com/design/sunrise-sunset-photos-sky-series-eric-cahan/

/*
Gradient sunset2 = Gradient({
  {.position  =  0, .color = RGB(   43,  0, 50)},  // Dark purple
  {.position  =127, .color = RGB(  129,  0,140)},  // Lighter purple
  {.position  =190, .color = RGB(  230,  0,255)},  // Bright purple
  {.position  =255, .color = RGB(   25,111,213)}; // Light blue
*/

Gradient sunset5 = Gradient({
    {.position = 0, .color = RGB(40, 0, 50)},       // Dark purple
    {.position = 150, .color = RGB(197, 70, 196)},  // Washed pink
    {.position = 200, .color = RGB(247, 170, 200)}, // Light washed pink
    {.position = 255, .color = RGB(237, 136, 80)}   // Orange
});

Gradient sunset6 = Gradient({
    {.position = 0, .color = RGB(40, 0, 50)},      // Dark purple
    {.position = 150, .color = RGB(230, 0, 255)},  // Light purple
    {.position = 210, .color = RGB(247, 170, 50)}, // Light Orange
    {.position = 255, .color = RGB(120, 60, 0)}    // Dark Orange
});

/*
Gradient sunset7 = Gradient({
  {.position  =  0, .color = RGB(   76, 55, 98)},  // Dark purple
  {.position  = 63, .color = RGB(   123, 66,123)}, // Light purple
  {.position  =127, .color = RGB(  194, 61, 92)},  // Light purple / red
  {.position  =191, .color = RGB(  216, 59, 62)},  // Light red
  {.position  =255, .color = RGB(  203, 44, 33)}}; // Dark red
});
*/

Gradient sunset8 = Gradient({
    {.position = 0, .color = RGB(60, 20, 80)},    // Dark purple
    {.position = 63, .color = RGB(100, 33, 123)}, // Light purple
    {.position = 127, .color = RGB(160, 30, 70)}, // Light purple / red
    {.position = 191, .color = RGB(216, 59, 62)}, // Light red
    {.position = 255, .color = RGB(203, 22, 16)}  // Dark red
});

PaletteColumn::Palette tunnel{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x03A688)},
        {.position = 63, .color = RGB(0x026873)},
        {.position = 127, .color = RGB(0x011F26)},
        {.position = 255, .color = RGB(0x025E73)},

    }),
    .primary = RGB(0x03A688),
    .secondary = RGB(0xF2668B),
    .highlight = RGB(255, 255, 255),
    .name = "Tunnel"};

PaletteColumn::Palette campfire{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x014040)},
        {.position = 63, .color = RGB(0x02735E)},
        {.position = 127, .color = RGB(0x03A678)},
        {.position = 255, .color = RGB(0xF27405)},

    }),
    .primary = RGB(0x014040),
    .secondary = RGB(0x731702),
    .highlight = RGB(255, 255, 255),
    .name = "Campfire"};

PaletteColumn::Palette greatBarrierReef{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x023059)},
        {.position = 63, .color = RGB(0x0487D9)},
        {.position = 127, .color = RGB(0x049DD9)},
        {.position = 255, .color = RGB(0x04B2D9)},

    }),
    .primary = RGB(0x023059),
    .secondary = RGB(0x04D9D9),
    .highlight = RGB(255, 255, 255),
    .name = "Great Barrier Reef"};

PaletteColumn::Palette candy{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0xD90452)},
        {.position = 63, .color = RGB(0xF205B3)},
        {.position = 127, .color = RGB(0xF2AE30)},
        {.position = 255, .color = RGB(0xF28D35)},

    }),
    .primary = RGB(0xD90452),
    .secondary = RGB(0xF26835),
    .highlight = RGB(255, 255, 255),
    .name = "candy"};

PaletteColumn::Palette retro{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x662400)},
        {.position = 63, .color = RGB(0xB33F00)},
        {.position = 200, .color = RGB(0xFF6B1A)},
        {.position = 255, .color = RGB(0x006663)},

    }),
    .primary = RGB(0x662400),
    .secondary = RGB(0x00B3AD),
    .highlight = RGB(255, 255, 255),
    .name = "retro"};

PaletteColumn::Palette coralTeal{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x3F7C85)},
        {.position = 63, .color = RGB(0x00CCBF)},
        {.position = 200, .color = RGB(0x72F2EB)},
        {.position = 255, .color = RGB(0x747E7E)},

    }),
    .primary = RGB(0x3F7C85),
    .secondary = RGB(0xFF5F5D),
    .highlight = RGB(255, 255, 255),
    .name = "coralTeal"};