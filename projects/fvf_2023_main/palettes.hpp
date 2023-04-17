#pragma once
#include "colours.h"
#include "generation/controlHub/paletteColumn.hpp"
#include "gradient.hpp"

// https://webgradients.com/
// https://color.adobe.com/create/color-wheel

RGB darken(RGB col, uint8_t dim)
{
    RGB result = col;
    result.dim(dim);
    return result;
}

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
    .name = "Sunset 3"};

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
    .name = "Sunset 4"};

// Some sunset color approximated from https://digitalsynopsis.com/design/sunrise-sunset-photos-sky-series-eric-cahan/


PaletteColumn::Palette sunset2{
    .gradient = new Gradient({
  {.position  =  0, .color = RGB(   43,  0, 50)},  // Dark purple
  {.position  =127, .color = RGB(  129,  0,140)},  // Lighter purple
  {.position  =190, .color = RGB(  230,  0,255)},  // Bright purple
  {.position  =255, .color = RGB(   25,111,213)} // Light blue
    }),
    .primary = RGB(  129,  0,140),
    .secondary = RGB(   25,111,213),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 2"};

PaletteColumn::Palette sunset5{
    .gradient = new Gradient({
    {.position = 0, .color = RGB(40, 0, 50)},       // Dark purple
    {.position = 150, .color = RGB(197, 70, 196)},  // Washed pink
    {.position = 200, .color = RGB(247, 170, 200)}, // Light washed pink
    {.position = 255, .color = RGB(237, 136, 80)}   // Orange
    }),
    .primary = RGB(247, 170, 200),
    .secondary = RGB(237, 136, 80),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 5"};


PaletteColumn::Palette sunset6{
    .gradient = new Gradient({
    {.position = 0, .color = RGB(40, 0, 50)},      // Dark purple
    {.position = 150, .color = RGB(230, 0, 255)},  // Light purple
    {.position = 210, .color = RGB(247, 170, 50)}, // Light Orange
    {.position = 255, .color = RGB(120, 60, 0)}    // Dark Orange
    }),
    .primary = RGB(230, 0, 255),
    .secondary =  RGB(120, 60, 0),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 6"};

PaletteColumn::Palette sunset7{
    .gradient = new Gradient({
  {.position  =  0, .color = RGB(   76, 55, 98)},  // Dark purple
  {.position  = 63, .color = RGB(   123, 66,123)}, // Light purple
  {.position  =127, .color = RGB(  194, 61, 92)},  // Light purple / red
  {.position  =191, .color = RGB(  216, 59, 62)},  // Light red
  {.position  =255, .color = RGB(  203, 44, 33)} // Dark red
    }),
    .primary = RGB(   76, 55, 98),
    .secondary =  RGB(  216, 59, 62),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 7"};

PaletteColumn::Palette sunset8{
    .gradient = new Gradient({
    {.position = 0, .color = RGB(60, 20, 80)},    // Dark purple
    {.position = 63, .color = RGB(100, 33, 123)}, // Light purple
    {.position = 127, .color = RGB(160, 30, 70)}, // Light purple / red
    {.position = 191, .color = RGB(216, 59, 62)}, // Light red
    {.position = 255, .color = RGB(203, 22, 16)}  // Dark red
    }),
    .primary = RGB(100, 33, 123),
    .secondary =   RGB(216, 59, 62),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 8"};

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
    .name = "Candy"};

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
    .name = "Retro"};

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
    .name = "Coral teal"};

PaletteColumn::Palette pinkSunset{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x6B1B5A)},
        {.position = 63, .color = RGB(0xB90852)},
        {.position = 200, .color = RGB(0xE7083F)},
        {.position = 255, .color = RGB(0xF62D2A)},

    }),
    .primary = RGB(0xB90852),
    .secondary = RGB(0xF7AF02),
    .highlight = RGB(255, 255, 255),
    .name = "Pink sunset"};

PaletteColumn::Palette salmonOnIce{
    .gradient = new Gradient({
        {.position = 0, .color = darken(RGB(0x3F464D), 100)},
        {.position = 100, .color = darken(RGB(0x2186C4), 150)},
        {.position = 180, .color = darken(RGB(0x2186C4), 250)},
        {.position = 190, .color = darken(RGB(0xFFF6E6), 230)},
        {.position = 200, .color = darken(RGB(0x2186C4), 50)},
        {.position = 250, .color = RGB(0x7ECEFC)},
        {.position = 255, .color = RGB(0xFFF6E6)},
    }),
    .primary = RGB(0x3F464D),
    .secondary = RGB(0xFF8066),
    .highlight = RGB(255, 255, 255),
    .name = "Salmon on ice"};

PaletteColumn::Palette blueOrange{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x0249DA)},
        {.position = 63, .color = RGB(0x0387D7)},
        {.position = 100, .color = RGB(0x039CC2)},
        {.position = 255, .color = RGB(0xF24603)},

    }),
    .primary = RGB(0x0387D7),
    .secondary = RGB(0xF47502),
    .highlight = RGB(255, 255, 255),
    .name = "Blue Orange"};

PaletteColumn::Palette purpleGreen{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x720EBF)},
        {.position = 63, .color = RGB(0xff0950)},
        {.position = 100, .color = darken(RGB(0xc913FF), 100)},
        {.position = 200, .color = RGB(0x260580)},
        {.position = 255, .color = RGB(0x8911E5)},
    }),
    .primary = RGB(0xFF00FF),
    .secondary = RGB(0x3EB200),
    .highlight = RGB(255, 255, 255),
    .name = "Purple green"};

// https://htmlcolors.com/gradient/31/beautiful-gradient
PaletteColumn::Palette heatmap2{
    .gradient = new Gradient({
        {.position = 0 * 255 / 10, .color = (RGB)HSL(240 * 255 / 360, 100 * 255 / 100, 0 * 255 / 100)},
        {.position = 1 * 255 / 10, .color = (RGB)HSL(254 * 255 / 360, 67 * 255 / 100, 13 * 255 / 100)},
        {.position = 2 * 255 / 10, .color = (RGB)HSL(275 * 255 / 360, 82 * 255 / 100, 22 * 255 / 100)},
        {.position = 3 * 255 / 10, .color = (RGB)HSL(297 * 255 / 360, 65 * 255 / 100, 26 * 255 / 100)},
        {.position = 4 * 255 / 10, .color = (RGB)HSL(324 * 255 / 360, 58 * 255 / 100, 36 * 255 / 100)},
        {.position = 5 * 255 / 10, .color = (RGB)HSL(345 * 255 / 360, 54 * 255 / 100, 47 * 255 / 100)},
        {.position = 6 * 255 / 10, .color = (RGB)HSL(8 * 255 / 360, 70 * 255 / 100, 54 * 255 / 100)},
        {.position = 7 * 255 / 10, .color = (RGB)HSL(25 * 255 / 360, 89 * 255 / 100, 52 * 255 / 100)},
        {.position = 8 * 255 / 10, .color = (RGB)HSL(38 * 255 / 360, 97 * 255 / 100, 51 * 255 / 100)},
        {.position = 9 * 255 / 10, .color = (RGB)HSL(49 * 255 / 360, 90 * 255 / 100, 61 * 255 / 100)},
        {.position = 10 * 255 / 10, .color = (RGB)HSL(49 * 255 / 360, 90 * 255 / 100, 61 * 255 / 100)},

    }),
    .primary = (RGB)HSL(345 * 255 / 360, 54 * 255 / 100, 47 * 255 / 100),
    .secondary = (RGB)HSL(49 * 255 / 360, 90 * 255 / 100, 61 * 255 / 100),
    .highlight = RGB(255, 255, 255),
    .name = "Heatmap 2"};

PaletteColumn::Palette peach{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0xfa709a)},
        {.position = 255, .color = RGB(0xfee140)},
    }),
    .primary = RGB(0xfa709a),
    .secondary = RGB(0xfee140),
    .highlight = RGB(255, 255, 255),
    .name = "Peach"};

PaletteColumn::Palette deepBlueOcean{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0x30cfd0)},
        {.position = 255, .color = RGB(0x330867)},
    }),
    .primary = RGB(0x30cfd0),
    .secondary = RGB(0x330867),
    .highlight = RGB(255, 255, 255),
    .name = "Deep blue ocean"};

PaletteColumn::Palette redSalvation{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0xf43b47)},
        {.position = 255, .color = RGB(0x453a94)},
    }),
    .primary = RGB(0xf43b47),
    .secondary = RGB(0x453a94),
    .highlight = RGB(255, 255, 255),
    .name = "Red salvation"};

PaletteColumn::Palette denseWater{
    .gradient = new Gradient({
        {.position = 0 * 255 / 100, .color = RGB(0x3ab5b0)},
        {.position = 31 * 255 / 100, .color = RGB(0x3d99be)},
        {.position = 100 * 255 / 100, .color = RGB(0x56317a)},
    }),
    .primary = RGB(0x3ab5b0),
    .secondary = RGB(0xF27811),
    .highlight = RGB(255, 255, 255),
    .name = "Dense water"};

PaletteColumn::Palette plumBath{
    .gradient = new Gradient({
        {.position = 0, .color = RGB(0xcc208e)},
        {.position = 255, .color = RGB(0x6713d2)},
    }),
    .primary = RGB(0x6713d2),
    .secondary = RGB(0x0CC2CC),
    .highlight = RGB(255, 255, 255),
    .name = "Plum bath"};