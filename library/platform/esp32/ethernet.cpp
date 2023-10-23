#include "platform/includes/ethernet.hpp"
#include "platform/includes/log.hpp"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_rom_gpio.h"
#include "esp_mac.h"

static const char *TAG = "ETH";

const gpio_num_t PIN_PHY_POWER = GPIO_NUM_12;

bool eth_connected = false;
bool eth_connecting = false;
esp_netif_t *eth_netif =NULL;
esp_eth_handle_t eth_handle = NULL;

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_ETH_GOT_IP:
            Log::info(TAG,"Got IP: %s", Ethernet::getIp().toString().c_str());

            eth_connected = true;
            eth_connecting = false;
            break;
        default:
            break;
        }
    }

    if (event_base == ETH_EVENT)
    {
        switch (event_id)
        {
        case ETHERNET_EVENT_START:
            Log::info(TAG,"ETH Started");
            break;
        case ETHERNET_EVENT_CONNECTED:
            Log::info(TAG,"ETH Connected");
            eth_connecting = true;
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            Log::info(TAG,"ETH Disconnected");
            eth_connected = false;
            eth_connecting = false;
            break;
        case ETHERNET_EVENT_STOP:
            Log::info(TAG,"ETH Stopped");
            eth_connected = false;
            eth_connecting = false;
            break;
        default:
            break;
        }
    }
}

void Ethernet::initialize()
{
    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create new default instance of esp-netif for Ethernet
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif = esp_netif_new(&cfg);

    // Init MAC and PHY configs to default
    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    mac_config.rx_task_stack_size = 4096;
    mac_config.rx_task_prio = 4;
    mac_config.flags = ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE;
    mac_config.sw_reset_timeout_ms = 1000;
    phy_config.phy_addr = 0;
    phy_config.reset_gpio_num = 12;
    esp_rom_gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_PHY_POWER, 1);
    vTaskDelay(pdMS_TO_TICKS(10));
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);

    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_START, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_STOP, &event_handler, NULL));

    //Log::info(TAG,"Duurt lang?1");
    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    //Log::info(TAG,"Duurt lang?2");
}

bool Ethernet::isConnected()
{
    return eth_connected;
}

bool Ethernet::isConnecting()
{
    return eth_connecting;
}

IPAddress Ethernet::getIp()
{
    esp_netif_ip_info_t info;
    esp_netif_get_ip_info(eth_netif, &info);
    return IPAddress::fromUint32(ntohl(info.ip.addr));
}

MacAddress Ethernet::getMac()
{
    MacAddress result;
    esp_read_mac(result.octets, ESP_MAC_ETH);
    return result;
}