#include "log.hpp"
#include <iostream>

void Log::info(const char * tag, const char* message)
{
    std::cout << message << "\n";
}