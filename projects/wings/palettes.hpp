#pragma once
#include "colours.h"
#include "gradient.hpp"

Gradient heatmap = Gradient({
    {.position = 0, .color = RGB(0, 0, 0)},        // Black
    {.position = 128, .color = RGB(255, 0, 0)},    // Red
    {.position = 224, .color = RGB(255, 255, 0)},  // Bright yellow
    {.position = 255, .color = RGB(255, 255, 255)} // Full white
});

// Some sunset color approximated from https://digitalsynopsis.com/design/sunrise-sunset-photos-sky-series-eric-cahan/
Gradient sunset1 = Gradient({
    {.position = 0, .color = RGB(8, 7, 72)},        // Dark blue
    {.position = 120, .color = RGB(78, 150, 212)},  // Light blue
    {.position = 160, .color = RGB(78, 150, 212)},  // Light blue
    {.position = 200, .color = RGB(233, 200, 246)}, // White
    {.position = 230, .color = RGB(205, 0, 226)},   // Purple
    {.position = 255, .color = RGB(205, 0, 255)}    // Purple
});

/*
Gradient sunset2 = Gradient({
  {.position  =  0, .color = RGB(   43,  0, 50)},  // Dark purple
  {.position  =127, .color = RGB(  129,  0,140)},  // Lighter purple
  {.position  =190, .color = RGB(  230,  0,255)},  // Bright purple
  {.position  =255, .color = RGB(   25,111,213)}; // Light blue
*/
Gradient sunset3 = Gradient({
    {.position = 0, .color = RGB(10, 20, 70)},      // Dark Blue
    {.position = 90, .color = RGB(32, 80, 118)},    // Lighter Blue
    {.position = 200, .color = RGB(170, 167, 153)}, // Grey
    {.position = 255, .color = RGB(200, 180, 50)}   // Greyish yellow
});

Gradient sunset4 = Gradient({
    {.position = 0, .color = RGB(60, 130, 197)},    // Light blue
    {.position = 160, .color = RGB(200, 220, 240)}, // White blue
    {.position = 210, .color = RGB(243, 170, 50)},  // Light orange
    {.position = 255, .color = RGB(100, 40, 0)}     // Dark orange
});

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