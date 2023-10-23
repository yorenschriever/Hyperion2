#include "NeoPixels.hpp"

NeoPixels::Timing NeoPixels::WS2811{
    300, 950,
    900, 350,
    300000};

NeoPixels::Timing NeoPixels::WS2812X{
    400, 850,
    800, 450,
    300000};

NeoPixels::Timing NeoPixels::SK6812{
    400, 850,
    800, 450,
    80000};

NeoPixels::Timing NeoPixels::Kpbs800{
    400, 850,
    800, 450,
    50000};

NeoPixels::Timing NeoPixels::Kpbs400{
    800, 1700,
    1600, 900,
    50000};

NeoPixels::Timing NeoPixels::APA106{
    400, 1250,
    1250, 400,
    50000};