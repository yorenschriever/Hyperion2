#pragma once

#include "ipAddress.hpp"

class Network
{
public:
    // in case of a .local address it resolves using mdns, and caches the result
    // otherwise it will do a regular hostname lookup

    // TODO: IPADdress::fromHostName already resolves for us.
    // resolve and resolveNoWait are not useful anymore.

    // i should make a hostnameCache that can resolve async

    static IPAddress *resolveNoWait(const char *hostname);
    static IPAddress *resolve(const char *hostname);
};
