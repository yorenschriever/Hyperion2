#include "platform/includes/ethernet.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/log.hpp"

static const char *TAG = "ETH";

void Ethernet::initialize()
{
    // nothing to do, assume ethernet is initialized by os
    Log::info(TAG, "Got IP: %s", Ethernet::getIp().toString().c_str());
}

IPAddress Ethernet::getIp()
{
    // TODO
    return IPAddress::fromHostName("localhost");
}

bool Ethernet::isConnected()
{
    return true;
}

bool Ethernet::isConnecting()
{
    return false;
}