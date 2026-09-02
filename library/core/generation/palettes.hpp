#pragma once
#include "colors.h"
#include "gradient.hpp"
#include "palette.hpp"

// https://webgradients.com/
// https://color.adobe.com/create/color-wheel

RGB darken(RGB col, uint8_t dim)
{
    RGB result = col;
    result.dim(dim);
    return result;
}

Palette heatmap{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0, 0, 0)},        // Black
        {.position = 128, .color = RGB(255, 0, 0)},    // Red
        {.position = 224, .color = RGB(255, 255, 0)},  // Bright yellow
        {.position = 255, .color = RGB(255, 255, 255)} // Full white
    }),
    .primary = RGB(255, 0, 0),
    .secondary = RGB(255, 255, 0),
    .highlight = RGB(255, 255, 255),
    .name = "Heatmap"};

Palette sunset1{
    .gradient = Gradient({
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

Palette sunset3{
    .gradient = Gradient({
        {.position = 0, .color = RGB(10, 20, 70)},      // Dark Blue
        {.position = 90, .color = RGB(32, 80, 118)},    // Lighter Blue
        {.position = 200, .color = RGB(170, 167, 153)}, // Grey
        {.position = 255, .color = RGB(200, 180, 50)}   // Greyish yellow
    }),
    .primary = RGB(32, 80, 118),
    .secondary = RGB(200, 180, 50),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 3"};

Palette sunset4{
    .gradient = Gradient({
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

Palette sunset2{
    .gradient = Gradient({
        {.position = 0, .color = RGB(43, 0, 50)},     // Dark purple
        {.position = 127, .color = RGB(129, 0, 140)}, // Lighter purple
        {.position = 190, .color = RGB(230, 0, 255)}, // Bright purple
        {.position = 255, .color = RGB(25, 111, 213)} // Light blue
    }),
    .primary = RGB(129, 0, 140),
    .secondary = RGB(25, 111, 213),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 2"};

Palette sunset5{
    .gradient = Gradient({
        {.position = 0, .color = RGB(40, 0, 50)},       // Dark purple
        {.position = 150, .color = RGB(197, 70, 196)},  // Washed pink
        {.position = 200, .color = RGB(247, 170, 200)}, // Light washed pink
        {.position = 255, .color = RGB(237, 136, 80)}   // Orange
    }),
    .primary = RGB(247, 170, 200),
    .secondary = RGB(237, 136, 80),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 5"};

Palette sunset6{
    .gradient = Gradient({
        {.position = 0, .color = RGB(40, 0, 50)},      // Dark purple
        {.position = 150, .color = RGB(230, 0, 255)},  // Light purple
        {.position = 210, .color = RGB(247, 170, 50)}, // Light Orange
        {.position = 255, .color = RGB(120, 60, 0)}    // Dark Orange
    }),
    .primary = RGB(230, 0, 255),
    .secondary = RGB(120, 60, 0),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 6"};

Palette sunset7{
    .gradient = Gradient({
        {.position = 0, .color = RGB(76, 55, 98)},    // Dark purple
        {.position = 63, .color = RGB(123, 66, 123)}, // Light purple
        {.position = 127, .color = RGB(194, 61, 92)}, // Light purple / red
        {.position = 191, .color = RGB(216, 59, 62)}, // Light red
        {.position = 255, .color = RGB(203, 44, 33)}  // Dark red
    }),
    .primary = RGB(76, 55, 98),
    .secondary = RGB(216, 59, 62),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 7"};

Palette sunset8{
    .gradient = Gradient({
        {.position = 0, .color = RGB(60, 20, 80)},    // Dark purple
        {.position = 63, .color = RGB(100, 33, 123)}, // Light purple
        {.position = 127, .color = RGB(160, 30, 70)}, // Light purple / red
        {.position = 191, .color = RGB(216, 59, 62)}, // Light red
        {.position = 255, .color = RGB(203, 22, 16)}  // Dark red
    }),
    .primary = RGB(100, 33, 123),
    .secondary = RGB(216, 59, 62),
    .highlight = RGB(255, 255, 255),
    .name = "Sunset 8"};

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

Palette campfire{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0x014040)},
        {.position = 63, .color = RGB(0x02735E)},
        {.position = 127, .color = RGB(0x03A678)},
        {.position = 255, .color = RGB(0xF27405)},

    }),
    .primary = RGB(0x03A678),
    .secondary = RGB(0x731702),
    .highlight = RGB(255, 255, 255),
    .name = "Campfire"};

Palette greatBarrierReef{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0x023059)},
        {.position = 63, .color = RGB(0x0487D9)},
        {.position = 127, .color = RGB(0x049DD9)},
        {.position = 255, .color = RGB(0x04B2D9)},

    }),
    .primary = RGB(0x023059),
    .secondary = RGB(0x04D9D9),
    .highlight = RGB(255, 255, 255),
    .name = "Great Barrier Reef"};

Palette candy{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0xD90452)},
        {.position = 63, .color = RGB(0xF205B3)},
        {.position = 127, .color = RGB(0xF2AE30)},
        {.position = 255, .color = RGB(0xF28D35)},

    }),
    .primary = RGB(0xD90452),
    .secondary = RGB(0xF26835),
    .highlight = RGB(255, 255, 255),
    .name = "Candy"};

Palette retro{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0x662400)},
        {.position = 63, .color = RGB(0xB33F00)},
        {.position = 200, .color = RGB(0xFF6B1A)},
        {.position = 255, .color = RGB(0x006663)},

    }),
    .primary = RGB(0x662400),
    .secondary = RGB(0x00B3AD),
    .highlight = RGB(255, 255, 255),
    .name = "Retro"};

Palette coralTeal{
    .gradient = Gradient({
        {.position = 0, .color = darken(RGB(0x3F7C85), 255)},
        {.position = 63, .color = RGB(0x00CCBF)},
        {.position = 200, .color = RGB(0x72F2EB)},
        {.position = 255, .color = RGB(0x747E7E)},

    }),
    .primary = RGB(0x3F7C85),
    .secondary = RGB(0xFF5F5D),
    .highlight = RGB(255, 255, 255),
    .name = "Coral teal"};

Palette pinkSunset{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0x6B1B5A)},
        {.position = 63, .color = RGB(0xB90852)},
        {.position = 200, .color = RGB(0xE7083F)},
        {.position = 255, .color = RGB(0xF62D2A)},

    }),
    .primary = RGB(0xB90852),
    .secondary = RGB(0xF7AF02),
    .highlight = RGB(255, 255, 255),
    .name = "Pink sunset"};

Palette salmonOnIce{
    .gradient = Gradient({
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

Palette blueOrange{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0x0249DA)},
        {.position = 63, .color = RGB(0x0387D7)},
        {.position = 100, .color = RGB(0x039CC2)},
        {.position = 255, .color = RGB(0xF24603)},

    }),
    .primary = RGB(0x0387D7),
    .secondary = RGB(0xF47502),
    .highlight = RGB(255, 255, 255),
    .name = "Blue Orange"};

Palette purpleGreen{
    .gradient = Gradient({
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
Palette heatmap2{
    .gradient = Gradient({
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

Palette peach{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0xfa709a)},
        {.position = 255, .color = RGB(0xfee140)},
    }),
    .primary = RGB(0xfa709a),
    .secondary = RGB(0xfee140),
    .highlight = RGB(255, 255, 255),
    .name = "Peach"};

Palette deepBlueOcean{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0x30cfd0)},
        {.position = 255, .color = RGB(0x330867)},
    }),
    .primary = RGB(0x30cfd0),
    .secondary = RGB(0x330867),
    .highlight = RGB(255, 255, 255),
    .name = "Deep blue ocean"};

Palette redSalvation{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0xf43b47)},
        {.position = 255, .color = RGB(0x453a94)},
    }),
    .primary = RGB(0xf43b47),
    .secondary = RGB(0x453a94),
    .highlight = RGB(255, 255, 255),
    .name = "Red salvation"};

Palette denseWater{
    .gradient = Gradient({
        {.position = 0 * 255 / 100, .color = RGB(0x3ab5b0)},
        {.position = 31 * 255 / 100, .color = RGB(0x3d99be)},
        {.position = 100 * 255 / 100, .color = RGB(0x56317a)},
    }),
    .primary = RGB(0x3ab5b0),
    .secondary = RGB(0xF27811),
    .highlight = RGB(255, 255, 255),
    .name = "Dense water"};

Palette plumBath{
    .gradient = Gradient({
        {.position = 0, .color = RGB(0xcc208e)},
        {.position = 255, .color = RGB(0x6713d2)},
    }),
    .primary = RGB(0x6713d2),
    .secondary = RGB(0x0CC2CC),
    .highlight = RGB(255, 255, 255),
    .name = "Plum bath"};

Palette matrix{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 50, 43)},
                          {.position = 127, .color = RGB(0, 140, 129)},
                          {.position = 190, .color = RGB(0, 255, 230)},
                          {.position = 255, .color = RGB(111, 213, 25)}}),
    .primary = RGB(0, 140, 129),
    .secondary = RGB(111, 213, 25),
    .highlight = RGB(255, 255, 255),
    .name = "Matrix"};

Palette orangeWhite{
    .gradient = Gradient({{.position = 0, .color = RGB(100, 100, 75)},
                          {.position = 255, .color = RGB(255, 100, 0)}}),
    .primary = RGB(255, 100, 0),
    .secondary = RGB(100, 100, 75),
    .highlight = RGB(255, 255, 255),
    .name = "Orange white"};

Palette rejected_hypothesis{
    .gradient = Gradient({{.position = 0, .color = RGB(30, 0, 10)},
                          {.position = 40, .color = RGB(139, 0, 20)},
                          {.position = 100, .color = RGB(220, 20, 60)},
                          {.position = 180, .color = RGB(255, 85, 0)},
                          {.position = 230, .color = RGB(255, 165, 0)},
                          {.position = 255, .color = RGB(255, 200, 100)}}),
    .primary = RGB(220, 20, 60),
    .secondary = RGB(255, 69, 0),
    .highlight = RGB(255, 240, 245),
    .name = "Rejected Hypothesis"};

Palette mad_science{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 40, .color = RGB(138, 43, 226)},
                          {.position = 120, .color = RGB(0, 255, 100)},
                          {.position = 200, .color = RGB(0, 255, 150)},
                          {.position = 255, .color = RGB(0, 255, 200)}}),
    .primary = RGB(0, 255, 100),
    .secondary = RGB(138, 43, 226),
    .highlight = RGB(255, 255, 200),
    .name = "Mad Science"};

Palette neon_fracture{
    .gradient = Gradient({{.position = 0, .color = RGB(20, 5, 10)},
                          {.position = 40, .color = RGB(180, 20, 40)},
                          {.position = 85, .color = RGB(255, 60, 50)},
                          {.position = 160, .color = RGB(255, 100, 120)},
                          {.position = 255, .color = RGB(255, 180, 160)}}),
    .primary = RGB(255, 45, 60),
    .secondary = RGB(220, 30, 90),
    .highlight = RGB(255, 200, 180),
    .name = "Neon Fracture"};

Palette chromatic_frost{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 40, .color = RGB(20, 60, 120)},
                          {.position = 100, .color = RGB(50, 130, 200)},
                          {.position = 180, .color = RGB(100, 200, 255)},
                          {.position = 230, .color = RGB(150, 230, 255)},
                          {.position = 255, .color = RGB(0, 220, 255)}}),
    .primary = RGB(100, 200, 255),
    .secondary = RGB(70, 150, 220),
    .highlight = RGB(240, 248, 255),
    .name = "Chromatic Frost"};

Palette bioluminescent_grove{
    .gradient = Gradient({{.position = 0, .color = RGB(10, 40, 20)},
                          {.position = 45, .color = RGB(0, 120, 80)},
                          {.position = 110, .color = RGB(20, 200, 120)},
                          {.position = 180, .color = RGB(34, 255, 120)},
                          {.position = 220, .color = RGB(100, 255, 150)},
                          {.position = 255, .color = RGB(200, 255, 100)}}),
    .primary = RGB(34, 255, 120),
    .secondary = RGB(0, 200, 100),
    .highlight = RGB(255, 255, 200),
    .name = "Bioluminescent Grove"};

Palette metamorphosis{
    .gradient = Gradient({{.position = 0, .color = RGB(20, 0, 40)},
                          {.position = 60, .color = RGB(138, 43, 226)},
                          {.position = 140, .color = RGB(255, 20, 147)},
                          {.position = 200, .color = RGB(255, 100, 200)},
                          {.position = 255, .color = RGB(255, 200, 100)}}),
    .primary = RGB(138, 43, 226),
    .secondary = RGB(255, 20, 147),
    .highlight = RGB(255, 255, 200),
    .name = "Metamorphosis"};

Palette electro_pulse{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 30, .color = RGB(255, 0, 128)},
                          {.position = 90, .color = RGB(255, 0, 200)},
                          {.position = 150, .color = RGB(0, 255, 255)},
                          {.position = 200, .color = RGB(0, 200, 255)},
                          {.position = 255, .color = RGB(0, 255, 200)}}),
    .primary = RGB(0, 255, 255),
    .secondary = RGB(255, 0, 128),
    .highlight = RGB(255, 255, 200),
    .name = "Electro Pulse"};

Palette volatile_synthesis{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 45, .color = RGB(138, 43, 226)},
                          {.position = 120, .color = RGB(0, 255, 136)},
                          {.position = 180, .color = RGB(255, 0, 200)},
                          {.position = 255, .color = RGB(255, 100, 0)}}),
    .primary = RGB(0, 255, 136),
    .secondary = RGB(138, 43, 226),
    .highlight = RGB(255, 255, 200),
    .name = "Volatile Synthesis"};

Palette volatile_reactions{
    .gradient = Gradient({{.position = 0, .color = RGB(20, 5, 40)},
                          {.position = 45, .color = RGB(80, 10, 150)},
                          {.position = 90, .color = RGB(200, 20, 180)},
                          {.position = 150, .color = RGB(255, 60, 100)},
                          {.position = 220, .color = RGB(255, 120, 40)},
                          {.position = 255, .color = RGB(255, 180, 0)}}),
    .primary = RGB(255, 30, 120),
    .secondary = RGB(100, 50, 200),
    .highlight = RGB(255, 200, 255),
    .name = "Volatile Reactions"};

Palette crimson_reverie{
    .gradient = Gradient({{.position = 0, .color = RGB(20, 0, 20)},
                          {.position = 64, .color = RGB(147, 51, 234)},
                          {.position = 160, .color = RGB(220, 20, 100)},
                          {.position = 220, .color = RGB(255, 100, 180)},
                          {.position = 255, .color = RGB(255, 200, 220)}}),
    .primary = RGB(220, 20, 60),
    .secondary = RGB(147, 51, 234),
    .highlight = RGB(255, 240, 245),
    .name = "Crimson Reverie"};

Palette ionosphere{
    .gradient = Gradient({{.position = 0, .color = RGB(10, 10, 30)},
                          {.position = 40, .color = RGB(80, 20, 120)},
                          {.position = 100, .color = RGB(160, 60, 200)},
                          {.position = 160, .color = RGB(0, 180, 255)},
                          {.position = 220, .color = RGB(100, 240, 255)},
                          {.position = 255, .color = RGB(0, 255, 200)}}),
    .primary = RGB(0, 200, 255),
    .secondary = RGB(100, 150, 255),
    .highlight = RGB(255, 255, 200),
    .name = "Ionosphere"};

Palette electroflux{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 40, .color = RGB(80, 0, 60)},
                          {.position = 100, .color = RGB(180, 0, 20)},
                          {.position = 180, .color = RGB(255, 60, 0)},
                          {.position = 240, .color = RGB(255, 120, 10)},
                          {.position = 255, .color = RGB(255, 180, 40)}}),
    .primary = RGB(255, 20, 0),
    .secondary = RGB(200, 10, 50),
    .highlight = RGB(255, 200, 100),
    .name = "Electroflux"};

Palette turboNoir{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 70, .color = RGB(0, 40, 80)},
                          {.position = 140, .color = RGB(0, 200, 180)},
                          {.position = 200, .color = RGB(80, 255, 200)},
                          {.position = 255, .color = RGB(0, 255, 180)}}),
    .primary = RGB(0, 255, 200),
    .secondary = RGB(0, 120, 180),
    .highlight = RGB(200, 255, 240),
    .name = "Turbo Noir"};

Palette plasmaRewind{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 55, .color = RGB(60, 0, 150)},
                          {.position = 110, .color = RGB(180, 0, 255)},
                          {.position = 180, .color = RGB(255, 80, 200)},
                          {.position = 230, .color = RGB(255, 200, 50)},
                          {.position = 255, .color = RGB(255, 220, 0)}}),
    .primary = RGB(180, 0, 255),
    .secondary = RGB(255, 80, 180),
    .highlight = RGB(255, 240, 200),
    .name = "Plasma Rewind"};

Palette cancan{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 64, .color = RGB(128, 0, 32)},
                          {.position = 128, .color = RGB(220, 0, 60)},
                          {.position = 192, .color = RGB(255, 20, 100)},
                          {.position = 255, .color = RGB(255, 0, 80)}}),
    .primary = RGB(220, 0, 60),
    .secondary = RGB(180, 0, 90),
    .highlight = RGB(255, 220, 230),
    .name = "Cancan"};

Palette laserGrid{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 60, .color = RGB(0, 10, 120)},
                          {.position = 130, .color = RGB(0, 180, 255)},
                          {.position = 200, .color = RGB(255, 0, 100)},
                          {.position = 255, .color = RGB(255, 0, 150)}}),
    .primary = RGB(0, 180, 255),
    .secondary = RGB(255, 0, 120),
    .highlight = RGB(200, 220, 255),
    .name = "Laser Grid"};

Palette vhsTracking{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 50, .color = RGB(80, 0, 120)},
                          {.position = 120, .color = RGB(200, 0, 80)},
                          {.position = 190, .color = RGB(255, 60, 0)},
                          {.position = 255, .color = RGB(255, 100, 0)}}),
    .primary = RGB(255, 30, 60),
    .secondary = RGB(160, 0, 120),
    .highlight = RGB(255, 220, 180),
    .name = "VHS Tracking"};

Palette midnightChrome{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 64, .color = RGB(20, 0, 80)},
                          {.position = 128, .color = RGB(180, 0, 180)},
                          {.position = 192, .color = RGB(255, 0, 255)},
                          {.position = 255, .color = RGB(255, 50, 200)}}),
    .primary = RGB(255, 0, 255),
    .secondary = RGB(120, 0, 200),
    .highlight = RGB(255, 200, 255),
    .name = "Midnight Chrome"};

Palette velvetThorn{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 80, .color = RGB(120, 0, 50)},
                          {.position = 150, .color = RGB(220, 0, 40)},
                          {.position = 200, .color = RGB(255, 0, 100)},
                          {.position = 255, .color = RGB(255, 0, 180)}}),
    .primary = RGB(220, 0, 40),
    .secondary = RGB(255, 0, 100),
    .highlight = RGB(255, 220, 240),
    .name = "Velvet Thorn"};

Palette scarletPromise{
    .gradient = Gradient({{.position = 0, .color = RGB(0, 0, 0)},
                          {.position = 60, .color = RGB(89, 2, 2)},
                          {.position = 120, .color = RGB(166, 3, 3)},
                          {.position = 175, .color = RGB(242, 29, 29)},
                          {.position = 225, .color = RGB(217, 121, 37)},
                          {.position = 255, .color = RGB(240, 150, 40)}}),
    .primary = RGB(242, 29, 29),
    .secondary = RGB(217, 121, 37),
    .highlight = RGB(255, 235, 180),
    .name = "ScarletPromise"};


Palette regalia{
    .gradient = Gradient({
        {.position = 0, .color = RGB(20, 0, 30)},
        {.position = 45, .color = RGB(100, 20, 150)},
        {.position = 120, .color = RGB(180, 80, 220)},
        {.position = 180, .color = RGB(220, 150, 50)},
        {.position = 255, .color = RGB(255, 215, 0)}
    }),
    .primary = RGB(218, 165, 32),
    .secondary = RGB(147, 51, 234),
    .highlight = RGB(255, 255, 200),
    .name = "Regalia"};


Palette aurelion{
    .gradient = Gradient({
        {.position = 0, .color = RGB(20, 10, 40)},
        {.position = 45, .color = RGB(75, 0, 130)},
        {.position = 120, .color = RGB(138, 43, 226)},
        {.position = 180, .color = RGB(200, 100, 255)},
        {.position = 220, .color = RGB(255, 180, 0)},
        {.position = 255, .color = RGB(255, 215, 0)}
    }),
    .primary = RGB(218, 165, 32),
    .secondary = RGB(138, 43, 226),
    .highlight = RGB(255, 255, 200),
    .name = "Aurelion"};

Palette regal_alchemy{
    .gradient = Gradient({
        {.position = 0, .color = RGB(25, 0, 51)},
        {.position = 45, .color = RGB(138, 43, 226)},
        {.position = 120, .color = RGB(218, 112, 214)},
        {.position = 200, .color = RGB(255, 215, 0)},
        {.position = 255, .color = RGB(255, 240, 45)}
    }),
    .primary = RGB(218, 165, 32),
    .secondary = RGB(186, 85, 211),
    .highlight = RGB(255, 255, 200),
    .name = "Regal Alchemy"};