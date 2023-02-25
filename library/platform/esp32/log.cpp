#include "log.hpp"
#include "esp_log.h"
#include <stdio.h>

char buffer [255];

void Log::info(const char* tag, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vsnprintf (buffer, sizeof(buffer), message, args);
    va_end(args);

    ESP_LOGI(tag, "%s", buffer);
}

void Log::error(const char* tag, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vsnprintf (buffer, sizeof(buffer), message, args);
    va_end(args);

    ESP_LOGE(tag, "%s", buffer);
}