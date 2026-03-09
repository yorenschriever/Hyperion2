#include "core/hyperion.hpp"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"

const gpio_num_t STATUS_LED_PIN = GPIO_NUM_3;

void initLed();
void updateLed(Hyperion *hyp);

bool isReceivingData = false;
class AnalyticsListener : public AnalyticsHub::Destination
{
    void sendAnalytics(const std::vector<AnalyticsHub::Analytics> &analyticsData) override
    {
        for (auto &source : analyticsData)
        {
            if (source.fps > 0 && source.lastFrameSize > 0)
            {
                isReceivingData = true;
                return;
            }
            isReceivingData = false;
        }
    }
};


int main()
{
    initLed();

    auto hyp = new Hyperion();

    Network::setHostName("hypernode1");
    AnalyticsHub::getInstance()->addAnalyticsDestination(new AnalyticsListener());

    // Neopixel pipes
    for (int i = 1; i <= 8; i++)
    {
        hyp->createChain(
            new UDPInput(9610 + i),
            new Analytics("NEOPIXEL_PORT_" + std::to_string(i)),
            new NeopixelOutput(i));
    }

    // DMX pipe
    hyp->createChain(
        new UDPInput(9619),
        new Analytics("DMX", 1),
        new DMXOutput());

    hyp->start(Hyperion::minimal);

    while (1)
    {
        updateLed(hyp);
        Thread::sleep(5);
    }
}

void initLed()
{
    esp_rom_gpio_pad_select_gpio(STATUS_LED_PIN);
    gpio_set_direction(STATUS_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(STATUS_LED_PIN, 0);
}

void updateLed(Hyperion *hyp)
{
    // If receiving data, flash fast
    if (isReceivingData)
    {
        bool flashFast = Utils::millis() % 100 < 50;
        gpio_set_level(STATUS_LED_PIN, flashFast);
        return;
    }

    // If connecting or waiting for IP, flash slow
    if (Ethernet::isConnecting())
    {
        bool flashSlow = Utils::millis() % 1000 < 500;
        gpio_set_level(STATUS_LED_PIN, flashSlow);
        return;
    }

    // If connected and on standby, blip every second
    if (Ethernet::isConnected())
    {
        bool blip = Utils::millis() % 1000 < 50;
        gpio_set_level(STATUS_LED_PIN, blip);
        return;
    }

    // Status led is off by default
    gpio_set_level(STATUS_LED_PIN, 0);
}

