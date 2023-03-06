#include "thread.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
        mapPurposeToCore(purpose));
}

void Thread::destroy()
{
    vTaskDelete(NULL);
}

int Thread::mapPurposeToCore(Purpose purpose)
{
    switch (purpose)
    {
    case Purpose::distribution:
        return 1;
    case Purpose::control:
    case Purpose::network:
    default:
        return 0;
    }
}