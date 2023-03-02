#include "platform/includes/utils.hpp"
#include "esp_timer.h"

unsigned long Utils::millis()
{
    return esp_timer_get_time() / 1000;
}

unsigned long Utils::micros()
{
    return esp_timer_get_time();
}

static void Utils::exit()
{
    ESP.restart();
}

static int Utils::random(int minimum, int maximum)
{
    return minimum + (esp_random() % (maximum - minimum));
}

int Utils::get_free_heap()
{
    return ESP.getFreeHeap();
}