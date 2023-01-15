#include "thread.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void Thread::sleep(unsigned int duration_ms)
{
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
}