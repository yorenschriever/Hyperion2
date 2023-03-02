#include "thread.hpp"
#include <chrono>
#include <thread>

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
    int uxPriority,
    TaskHandle_t * const pvCreatedTask
){
    //auto threadHandle = std::thread(pvCreatedTask, pvParameters);
    //threadHandle.detach();
    return 0;
}

void Thread::destroy(){}