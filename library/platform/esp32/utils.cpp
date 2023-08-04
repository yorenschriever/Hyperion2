
#include "platform/includes/utils.hpp"

#include "esp_timer.h"
#include "esp_system.h"
#include "esp_random.h"
#include "esp_heap_caps.h"

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

void *Utils::malloc_dma(int size)
{
    return heap_caps_malloc(size, MALLOC_CAP_DMA| MALLOC_CAP_32BIT);
}

void *Utils::realloc_dma(void* ptr, int size)
{
    return heap_caps_realloc(ptr, size, MALLOC_CAP_DMA| MALLOC_CAP_32BIT);
}