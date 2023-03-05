#include "platform/includes/utils.hpp"
#include <chrono>
#include <stdlib.h>

using namespace std::chrono;

unsigned long Utils::millis()
{  
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

unsigned long Utils::micros()
{
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

void Utils::exit()
{
    exit();
}

int Utils::random(int minimum, int maximum)
{
    return minimum + (rand() % (maximum - minimum));
}

int Utils::get_free_heap()
{
    return 0;
}

void *Utils::malloc_dma(int size)
{
    return malloc(size);
}

void *Utils::realloc_dma(void* ptr, int size)
{
    return realloc(ptr, size);
}