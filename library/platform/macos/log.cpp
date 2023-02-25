#include "log.hpp"
#include <iostream>

#include <stdio.h>
#include <stdarg.h>
#include <string>

using namespace std;

void Log::info(const char * tag, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    vprintf(message,args);
    va_end(args);
    std::printf("\n"); 
}

void Log::error(const char * tag, const char* message, ...)
{
    std::printf("\x1b[31m"); //set color to red
    va_list args;
    va_start(args, message);
    vprintf(message,args);
    va_end(args);
    std::printf("\n"); 
    std::printf("\x1b[0m"); //set color to normal
}