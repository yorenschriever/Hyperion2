#include "network.hpp"
#include "log.hpp"

const char* TAG = "MDNS";

void Network::setHostName(const char* hostname)
{
    Log::error(TAG,"mdns not supported on this platform. Cannot set hostname to: %s.local",hostname);
    Network::hostname = hostname;
}

const char* Network::getHostName() { return hostname; }

const char* Network::hostname = nullptr;

