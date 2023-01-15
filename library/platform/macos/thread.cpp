#include "thread.hpp"
#include <chrono>
#include <thread>

void Thread::sleep(unsigned int duration_ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
}