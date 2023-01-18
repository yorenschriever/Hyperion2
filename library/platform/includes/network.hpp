#pragma once
#pragma once

#include "ipAddress.hpp"

class Network
{
public:
    //in case of a .local address it resolves using mdns, and caches the result
    //otherwise it will do a regular hostname lookup
    static IPAddress* ResolveNoWait(const char* hostname);
    static IPAddress* Resolve(const char* hostname);
};
