#pragma once

#include "ipAddress.hpp"

class Ethernet
{
public:
    // starts the network
    static void initialize();

    // tells if the network is connected
    static bool isConnected();
    static bool isConnecting();

    static IPAddress getIp();
};
