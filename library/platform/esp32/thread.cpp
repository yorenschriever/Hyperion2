#include "thread.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void Thread::sleep(unsigned int duration_ms)
{
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
}

int Thread::create(
    TaskFunction_t pvTaskCode,
    const char * const pcName,
    Purpose purpose,
    const uint32_t usStackDepth,
    void * const pvParameters,
    int uxPriority,
    TaskHandle_t * const pvCreatedTask
){
    return xTaskCreatePinnedToCore(
        pvTaskCode,
        pcName,
        usStackDepth,
        pvParameters,
        uxPriority,
        (TaskHandle_t*) pvCreatedTask,
        (purpose == Thread::Purpose::distribution) ? 1:0
    )
}

void Thread::destroy(){
    vTaskDelete(NULL);
}