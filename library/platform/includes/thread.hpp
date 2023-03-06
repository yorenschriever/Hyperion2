#pragma once

#include <stdint.h>

class Thread
{
public:
    typedef void (*TaskFunction_t)( void * );
    typedef void * TaskHandle_t;

    enum Purpose { distribution, network, control };

    static void sleep(unsigned int duration_ms);

    static int create(
        TaskFunction_t pvTaskCode,
        const char * const pcName,
        Purpose purpose,
        const uint32_t usStackDepth,
        void * const pvParameters,
        int uxPriority
    );

    static void destroy();

private:
    static int mapPurposeToCore(Purpose purpose);
};