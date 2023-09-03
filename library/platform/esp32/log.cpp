#include "log.hpp"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

char buffer[255];
const SemaphoreHandle_t sema = xSemaphoreCreateMutex();
#define WAIT_FOR_SEMAPHORE portMAX_DELAY

void Log::info(const char *tag, const char *message, ...)
{
    if (!xSemaphoreTake(sema, WAIT_FOR_SEMAPHORE))
        return;

    va_list args;
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    ESP_LOGI(tag, "%s", buffer);

    xSemaphoreGive(sema);
}

void Log::error(const char *tag, const char *message, ...)
{
    if (!xSemaphoreTake(sema, WAIT_FOR_SEMAPHORE))
        return;

    va_list args;
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    ESP_LOGE(tag, "%s", buffer);

    xSemaphoreGive(sema);
}