#include "log.hpp"
#include "esp_log.h"

void Log::info(const char* tag, const char* message)
{
    ESP_LOGI(tag, "%s",message);
}