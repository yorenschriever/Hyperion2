#pragma once

#include "platform/includes/ipAddress.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/thread.hpp"
#include <map>
#include <string>

class HostnameCache
{
public:
    static IPAddress *lookup(const char *hostname)
    {
        std::map<std::string, CacheItem>::iterator it = cache.find(hostname);
        if (it == cache.end())
        {
            // start lookup
            cache.insert(
                std::pair<std::string, HostnameCache::CacheItem>(
                    hostname,
                    HostnameCache::CacheItem{IPAddress::fromUint32(0), Utils::millis(), false}));

            // Log::info("HOSTNAME_CACHE", "hostname cache item created");
            Thread::create(resolveTask, "ResolveTask", Thread::Purpose::network, 3000, (void *)hostname, 0);
            return NULL;
        }

        auto cacheHit = it->second;
        if (!cacheHit.found)
        {
            if ((Utils::millis() - cacheHit.updated) < 5000)
            {
                // last hostname query returned nothing, and was less than 5 seconds ago. Do not ask again yet
                // Log::info("HOSTNAME_CACHE","less than 5 sec. waiting");
                return NULL;
            }
            Log::info("HOSTNAME_CACHE", "item was queried before, but not found, retry");
            cache.find(hostname)->second.updated = Utils::millis();

            Thread::create(resolveTask, "ResolveTask", Thread::Purpose::network, 3000, (void *)hostname, 0);
            return NULL;
        }

        if (Utils::millis() - cacheHit.updated > 60000)
        {
            // entry is older than 1 minute, refresh
            Log::info("HOSTNAME_CACHE", "refreshing hostname cache item for %s", hostname);
            cache.find(hostname)->second.updated = Utils::millis();
            Thread::create(resolveTask, "ResolveTask", Thread::Purpose::network, 3000, (void *)hostname, 0);
        }

        // Log::info("HOSTNAME_CACHE","return cached ip %s\r\n",cacheHit.ip.toString().c_str());
        return &it->second.ip;
    }

    static void refresh(const char *hostname)
    {
        auto it = cache.find(hostname);
        if (it == cache.end())
            return;
        if (Utils::millis() - it->second.updated > 5000)
            return;

        Log::info("HOSTNAME_CACHE","refreshing %s",hostname);

        it->second.updated = Utils::millis();
        Thread::create(resolveTask, "ResolveTask", Thread::Purpose::network, 3000, (void *)hostname, 0);
    }
private:
    struct CacheItem
    {
        IPAddress ip;
        unsigned long updated;
        bool found;
    };
    static std::map<std::string, CacheItem> cache;

    static void resolveTask(void *pvParameters)
    {
        const char *hostname = static_cast<const char *>(pvParameters);
        auto item = cache.find(hostname);

        if (item == cache.end())
        {
            Log::error("HOSTNAME_CACHE", "item not found");
        }
        else
        {
            item->second.ip = IPAddress::fromHostname(hostname);
            item->second.updated = Utils::millis();
            item->second.found = true;
        }

        Thread::destroy();
    }
};

std::map<std::string, HostnameCache::CacheItem> HostnameCache::cache;
