#include "network.hpp"
#include "log.hpp"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "misc/mdns/mdns.h"

const char* TAG = "MDNS";

void Network::setHostName(const char* hostname)
{
    Log::info(TAG,"Setting hostname to: %s.local",hostname);


    //store the hostname. the actual mdns_hostname_set takes places after ETH has acquired an ip address
    //ethernet.cpp takes care of that.
    Network::hostname = hostname;
}

const char* Network::getHostName() { return hostname; }

const char* Network::hostname = nullptr;