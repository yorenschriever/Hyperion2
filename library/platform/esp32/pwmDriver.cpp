#include "pwmDriver.hpp"
#include "pwmDriver-esp.hpp"
#include "log.hpp"

PWMDriver * instance = nullptr;

PWMDriver *PWMDriver::getInstance()
{
    if (!instance)
        instance = new PWMDriver_ESP();

    return instance;
}