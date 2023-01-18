#include "platform/includes/ethernet.hpp"

// // #ifndef LWIP_OPEN_SRC
// // #define LWIP_OPEN_SRC
// // #endif
// // #define CONFIG_ETH_ENABLED

// // #include <functional>
// // #include "esp_wifi.h"

// // #include <Arduino.h>
// // #include "ethernet.h"
// // #include "debug.h"
// // #include "mdns.h"
// // #include <ESPmDNS.h>
// // #include <lwip/netdb.h>

// // #include "esp_system.h"
// // #include "esp_event.h"
// // #include "esp_eth.h"
// // #include "esp_eth_phy.h"
// // #include "esp_eth_mac.h"
// // #include "esp_eth_com.h"
// // #include "lwip/err.h"
// // #include "lwip/dns.h"

// // //config to get the olimex poe board working with ETH
// // #define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
// // #define ETH_PHY_POWER 12

// // esp_eth_handle_t Ethernet::eth_handle = NULL;
// // bool Ethernet::eth_connected = false;
// // bool Ethernet::eth_connecting = false;
// // const char *Ethernet::hostname;
// // std::map<std::string, Ethernet::hostnameCacheItem> Ethernet::hostnameCache;

// // extern void tcpipInit();

// // #include "ethernet_init.h"
#include "esp_eth_driver.h"
#include "esp_eth_mac.h"

#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

void Ethernet::Initialize(const char *hostname)
{

    //esp_err_t ret = ESP_OK;
    esp_eth_handle_t eth_handle;
    //uint8_t eth_cnt = 0;

    //ESP_GOTO_ON_FALSE(eth_handles_out != NULL && eth_cnt_out != NULL, ESP_ERR_INVALID_ARG,
    //                    err, TAG, "invalid arguments: initialized handles array or number of interfaces");
    //eth_handles = (esp_eth_handle_t*) calloc(INTERNAL_ETHERNETS_NUM, sizeof(esp_eth_handle_t));
    //ESP_GOTO_ON_FALSE(eth_handles != NULL, ESP_ERR_NO_MEM, err, TAG, "no memory");

    // Init common MAC and PHY configs to default
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    // Update PHY config based on board specific configuration
    //phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;
    //phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;

    phy_config.phy_addr = 1;
    phy_config.reset_gpio_num = 5;

    // Init vendor specific MAC config to default
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();

    // Update vendor specific MAC config based on board configuration
    esp32_emac_config.smi_mdc_gpio_num = 23;
    esp32_emac_config.smi_mdio_gpio_num = 18;
    // Create new ESP32 Ethernet MAC instance
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);


    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);

    // Init Ethernet driver to default and install it
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_driver_install(&config, &eth_handle);

    //eth_handles[eth_cnt] = eth_init_internal(NULL, NULL);
    //ESP_GOTO_ON_FALSE(eth_handles[eth_cnt], ESP_FAIL, err, TAG, "internal Ethernet init failed");
    //eth_cnt++;

    //*eth_handles_out = eth_handles;
    //*eth_cnt_out = eth_cnt;

    // ret;

// err:

//     if (eth_handle != NULL) {
//         esp_eth_driver_uninstall(eth_handle);
//     }
//     if (mac != NULL) {
//         mac->del(mac);
//     }
//     if (phy != NULL) {
//         phy->del(phy);
//     }

//     return ret;

}


// // #############

//     Ethernet::hostname = hostname;

//     // This delay makes sure that not all nodes will startup at exactly the same time when you flip the master power switch
//     // Routers were having trouble when a large amount of nodes started communicating at exactly the same time.
//     delay(100 + (esp_random() & 0xFF) * 5);

//     tcpipInit();

//     tcpip_adapter_set_default_eth_handlers();
//     eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
//     mac_config.smi_mdc_gpio_num = 23;
//     mac_config.smi_mdio_gpio_num = 18;
//     mac_config.sw_reset_timeout_ms = 1000; // this timeout is increased for stability
//     esp_eth_mac_t *eth_mac = NULL;

//     eth_mac = esp_eth_mac_new_esp32(&mac_config);

//     if (eth_mac == NULL)
//     {
//         Debug.println("Eth error: new mac failed");
//         log_e("esp_eth_mac_new_esp32 failed");
//         return;
//     }

//     eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
//     phy_config.phy_addr = 0;
//     phy_config.reset_gpio_num = 12;
//     esp_eth_phy_t *eth_phy = NULL;

//     eth_phy = esp_eth_phy_new_lan8720(&phy_config);

//     if (eth_phy == NULL)
//     {
//         Debug.println("Eth error: new phy failed");
//         log_e("esp_eth_phy_new failed");
//         return;
//     }

//     eth_handle = NULL;
//     esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(eth_mac, eth_phy);

//     if (esp_eth_driver_install(&eth_config, &eth_handle) != ESP_OK || eth_handle == NULL)
//     {
//         Debug.println("Eth error: driver install failed");
//         log_e("esp_eth_driver_install failed");
//         return;
//     }

//     if (esp_eth_start(eth_handle) != ESP_OK)
//     {
//         Debug.println("Eth error: eth start failed");
//         log_e("esp_eth_start failed");
//         return;
//     }

//     //     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     //     //ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
//     //     //ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
//     // esp_event_loop_init(event_handler2, nullptr);

//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_START, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_STOP, &event_handler, NULL));

//     Debug.println("Eth initialized");
// }

// // for maximum compatibility with other hardware (routers, pioneer gear, (hyper)linked operation),
// // it is not advised to set a fixed ip address. The default approach is to let the device request an ip from
// // the dhcp server, and self assign an autoip otherwise.
// // if you want to target this device specifically, set HostName in the config
// // this sets the hostname and a mdns entry.
// bool Ethernet::SetFixedIp(IPAddress ip, IPAddress gateway, IPAddress subnet)
// {
//     tcpip_adapter_ip_info_t info;
//     info.ip.addr = static_cast<uint32_t>(ip);
//     info.gw.addr = static_cast<uint32_t>(gateway);
//     info.netmask.addr = static_cast<uint32_t>(subnet);

//     ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_ETH));
//     // ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &info));
//     bool result = (tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &info) == ESP_OK);

//     Debug.println("");

//     Debug.println("Eth connected");
//     Debug.println("IP address: ");
//     Debug.println(ETH.localIP());

//     return result;
// }

// bool Ethernet::isConnected()
// {
//     return eth_connected;
// }

// bool Ethernet::isConnecting()
// {
//     return eth_connecting;
// }

// IPAddress Ethernet::GetIp()
// {
//     tcpip_adapter_ip_info_t ip;
//     if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_ETH, &ip))
//     {
//         return IPAddress();
//     }
//     return IPAddress(ip.ip.addr);
// }

// uint8_t *Ethernet::GetMac(uint8_t *mac)
// {
//     if (!mac)
//     {
//         return NULL;
//     }
//     esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac);
//     return mac;
// }

// void Ethernet::event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
// {
//     if (event_base == IP_EVENT)
//     {
//         switch (event_id)
//         {
//         case IP_EVENT_ETH_GOT_IP:
//             Debug.print("Ethernet IPv4: ");
//             Debug.print(GetIp());
//             eth_connected = true;
//             eth_connecting = false;

//             if (Ethernet::hostname)
//             {
//                 StartMdnsService(Ethernet::hostname);
//             }

//             break;
//         case IP_EVENT_STA_GOT_IP:
//             Debug.printf("Wifi connected.\r\n");
//             if (Ethernet::hostname)
//             {
//                 StartMdnsService(Ethernet::hostname);
//             }
//             break;
//         default:
//             break;
//         }
//     }

//     if (event_base == ETH_EVENT)
//     {
//         switch (event_id)
//         {
//         case ETHERNET_EVENT_START:
//             Debug.println("ETH Started");
//             // set eth hostname here
//             if (Ethernet::hostname)
//                 ETH.setHostname(Ethernet::hostname);
//             hostnameCache.clear();
//             break;
//         case ETHERNET_EVENT_CONNECTED:
//             Debug.println("ETH Connected");
//             eth_connecting = true;
//             break;
//         case ETHERNET_EVENT_DISCONNECTED:
//             Debug.println("ETH Disconnected");
//             eth_connected = false;
//             eth_connecting = false;
//             break;
//         case ETHERNET_EVENT_STOP:
//             Debug.println("ETH Stopped");
//             eth_connected = false;
//             eth_connecting = false;
//             break;
//         default:
//             break;
//         }
//     }

//     if (event_base == WIFI_EVENT)
//     {
//         switch (event_id)
//         {
//         case WIFI_EVENT_AP_START:
//             Debug.printf("Wifi connected.\r\n");
//             if (Ethernet::hostname)
//             {
//                 StartMdnsService(Ethernet::hostname);
//             }
//             break;
//         default:
//             break;
//         }
//     }
// }

// void Ethernet::StartMdnsService(const char *name)
// {
//     if (!MDNS.begin(name))
//     {
//         Debug.println("Error setting up MDNS responder!");
//     }
//     else
//     {
//         Debug.printf("Hostname set: %s\r\n", name);
//     }
// }

// }