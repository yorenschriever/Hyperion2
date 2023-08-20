#include "thread.hpp"
#include "platform/includes/log.hpp"
#include <chrono>
#include <thread>

static const char *TAG = "THREAD";

void Thread::sleep(unsigned int duration_ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
}

int Thread::create(
    TaskFunction_t pvTaskCode,
    const char * const pcName,
    Purpose purpose,
    const uint32_t usStackDepth,
    void * const pvParameters,
    int uxPriority
){
    //Log::info(TAG,"creating thread");
    auto threadHandle = std::thread(pvTaskCode, pvParameters);
    threadHandle.detach();
    return 0;
}

void Thread::destroy(){}