#pragma once

#include "platform/includes/ipAddress.hpp"
#include "platform/includes/utils.hpp"
#include <map>
#include <string>

class HostnameCache
{
    public:
        static IPAddress* lookup(const char* hostname);

    private:
        struct CacheItem {IPAddress ip; unsigned long updated; bool found;};
        static std::map<std::string, CacheItem> cache; 

        static void resolveTask(void * pvParameters);
        // static bool resolveTaskStarted=false;
};