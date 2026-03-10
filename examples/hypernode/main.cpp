#include "core/hyperion.hpp"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"

const gpio_num_t STATUS_LED_PIN = GPIO_NUM_3;

class AnalyticsLed : public AnalyticsHub::Destination
{
    public:
    bool isReceivingData = false;

    AnalyticsLed() { 
        AnalyticsHub::getInstance()->addAnalyticsDestination(this);
    }

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

void initLed();
void updateLed(Hyperion *hyp, AnalyticsLed *analytics);

std::string analyticsName(uint16_t port){
    return Network::getHostName() + std::string(".local:") + std::to_string(port);
}

int main()
{
    initLed();

    auto hyp = new Hyperion();

    const char *hostName = "hypernode1";
    Network::setHostName(hostName);
   
    // Neopixel pipes
    for (int i = 1; i <= 8; i++)
    {
        auto repeatFallback = new RepeatFallback();
        int port = 9610 + i;
        hyp->createChain(
            new UDPInput(port),
            new Analytics(analyticsName(port)),
            repeatFallback->sink);
        hyp->createChain(repeatFallback->source,
            new NeopixelOutput(i));
    }

    // DMX pipe
    auto repeatFallback = new RepeatFallback();
    int dmxPort = 9619;
    hyp->createChain(
        new UDPInput(dmxPort),
        new Analytics(analyticsName(dmxPort)),
        repeatFallback->sink);
    hyp->createChain(repeatFallback->source,
        new DMXOutput());

    hyp->start(Hyperion::minimal);

    // UDP sockets can only initialize after the network is up,
    // so we create AnalyticsLed after hyp->start().
    auto analytics = new AnalyticsLed();
    new AnalyticsUDP();

    while (1)
    {
        updateLed(hyp, analytics);
        Thread::sleep(5);
    }
}

void initLed()
{
    esp_rom_gpio_pad_select_gpio(STATUS_LED_PIN);
    gpio_set_direction(STATUS_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(STATUS_LED_PIN, 0);
}

void updateLed(Hyperion *hyp, AnalyticsLed *analytics)
{
    // If receiving data, flash fast
    if (analytics->isReceivingData)
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
        int doubleBlipMode = Utils::millis() % 1000;
        bool doubleBlip = doubleBlipMode < 50 || (doubleBlipMode > 150 && doubleBlipMode < 200);
        gpio_set_level(STATUS_LED_PIN, doubleBlip);
        return;
    }

    bool blip = Utils::millis() % 1000 < 50;
    gpio_set_level(STATUS_LED_PIN, blip);

}

