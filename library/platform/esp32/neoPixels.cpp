#include "neoPixels.hpp"
#include "neoPixels-esp.hpp"
#include "log.hpp"

NeoPixels *NeoPixels::createInstance()
{
    return new NeoPixelsEsp();
}