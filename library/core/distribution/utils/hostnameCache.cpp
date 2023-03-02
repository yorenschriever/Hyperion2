#include "hostnameCache.hpp"

#include "platform/includes/utils.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"

static const char* TAG = "HOSTNAME_CACHE";

std::map<std::string, HostnameCache::CacheItem> HostnameCache::cache;

IPAddress *HostnameCache::lookup(const char *hostname)
{
    std::map<std::string, CacheItem>::iterator it = cache.find(hostname);
    if (it == cache.end())
    {
        // start lookup
        cache[hostname] = CacheItem::CacheItem{IPAddress::fromUint32(0), Utils::millis(), false};
        // Log::info(TAG,"hostname item created");
        Thread::create(resolveTask,"ResolveTask", Thread::Purpose::network, 3000, (void *)hostname, 0, NULL);
        return NULL;
    }

    auto cacheHit = it->second;
    if (!cacheHit.found)
    {
        if ((Utils::millis() - cacheHit.updated) < 5000)
        {
            // last hostname query returned nothing, and was less than 5 seconds ago. Do not ask again yet
            // Log::info(TAG,"less than 5 sec. waiting");
            return NULL;
        }
        // Log::info(TAG,"item was queried before, but not found, retry");
        cache[hostname].updated = Utils::millis();
        Thread::create(resolveTask,"ResolveTask", Thread::Purpose::network, 3000, (void *)hostname, 0, NULL);
        return NULL;
    }

    if (Utils::millis() - cacheHit.updated > 10000)
    {
        // entry is older than 1 minute, refresh
        // Log::info(TAG,"refreshing hostname item");
        cache[hostname].updated = Utils::millis();
        Thread::create(resolveTask,"ResolveTask", Thread::Purpose::network, 3000, (void *)hostname, 0, NULL);
    }

    // Log::info(TAG,"return cached ip %s\r\n",cacheHit.ip.toString().c_str());
    return &it->second.ip;
}

void HostnameCache::resolveTask(void *pvParameters)
{
    const char *hostname = static_cast<const char *>(pvParameters);
    cache[hostname].ip = IPAddress::fromHostname(hostname);
    cache[hostname].updated = Utils::millis();
    cache[hostname].found = true;

    Thread::destroy();
}