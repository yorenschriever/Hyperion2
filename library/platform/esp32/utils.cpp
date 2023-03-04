
#include "platform/includes/utils.hpp"

#include "esp_timer.h"
#include "esp_system.h"
#include "esp_random.h"

unsigned long Utils::millis()
{
    return esp_timer_get_time() / 1000;
}

unsigned long Utils::micros()
{
    return esp_timer_get_time();
}

void Utils::exit()
{
    esp_restart();
}

int Utils::random(int minimum, int maximum)
{
    return minimum + (esp_random() % (maximum - minimum));
}

float Utils::random_f()
{
    return esp_random();
}

int Utils::get_free_heap()
{
    return esp_get_free_heap_size();
}