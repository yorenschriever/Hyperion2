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
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        Log::error(TAG,"MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    //set default instance
    mdns_instance_name_set("Hyperion");
}

