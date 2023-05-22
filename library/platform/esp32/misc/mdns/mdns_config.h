#pragma once

// config settings from the example that i hardcoded here

#define CONFIG_MDNS_MAX_INTERFACES 3
#define CONFIG_MDNS_MAX_SERVICES 3
#define CONFIG_MDNS_TIMER_PERIOD_MS 100
#define CONFIG_MDNS_TASK_STACK_SIZE 4096
#define CONFIG_MDNS_TASK_PRIORITY 1
#define CONFIG_MDNS_TASK_AFFINITY 1 
#define CONFIG_MDNS_SERVICE_ADD_TIMEOUT_MS 2000

#define CONFIG_MDNS_PREDEF_NETIF_ETH 1

// at the moment i dont build with wifi support yet
// but mdns needs some constant from the wifi header files, so i mock them here
// remove these lines once wifi is included in the build
#define WIFI_EVENT -1
#define WIFI_EVENT_STA_CONNECTED -1
#define WIFI_EVENT_STA_DISCONNECTED -2
#define WIFI_EVENT_AP_START -3
#define WIFI_EVENT_AP_STOP -4