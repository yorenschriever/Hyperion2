#pragma once

#include <stdarg.h>
#include <cstring>

class Log
{
public:
    static void info(const char *tag, const char *message, ...);
    static void error(const char *tag, const char *message, ...);
};