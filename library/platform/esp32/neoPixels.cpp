#include "neoPixels.hpp"
#include "neoPixels-esp-legacyRMT.hpp"
#include "log.hpp"

NeoPixels *NeoPixels::createInstance()
{
    return new NeoPixelsEspLegacyRMT();
}