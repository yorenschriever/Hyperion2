#include "thread.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int mapThreadPurposeToCore(Thread::Purpose purpose)
{
    switch (purpose)
    {
    case Thread::Purpose::distribution:
        return 0;
    case Thread::Purpose::control:
    case Thread::Purpose::network:
    default:
        return 0;
    }
}

void Thread::sleep(unsigned int duration_ms)
{
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
}

int Thread::create(
    TaskFunction_t pvTaskCode,
    const char *const pcName,
    Purpose purpose,
    const uint32_t usStackDepth,
    void *const pvParameters,
    int uxPriority)
{
    return xTaskCreatePinnedToCore(
        pvTaskCode,
        pcName,
        usStackDepth,
        pvParameters,
        uxPriority,
        NULL,
        mapThreadPurposeToCore(purpose));
}

void Thread::destroy()
{
    vTaskDelete(NULL);
}

