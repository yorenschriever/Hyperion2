#include "pwmDriver.hpp"
#include "log.hpp"

PWMDriver* PWMDriver::getInstance()
{
    Log::error("PWM","PWM not supported on this platform. PWM data will not be sent");
    return nullptr;
}