#include "core/hyperion.hpp"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"

const gpio_num_t STATUS_LED_PIN = GPIO_NUM_3;

void initLed();
void updateLed(Hyperion *hyp);

int main()
{
  initLed();

  auto hyp = new Hyperion();

  Network::setHostName("hypernode1");

  //Neopixel pipes
  for (int i = 1; i <= 8; i++)
  {
    auto pipe = new Pipe(
        new UDPInput(9610 + i),
        new NeopixelOutput(i));
    hyp->addPipe(pipe);
  }

  //DMX pipe
  {
    auto pipe = new Pipe(
        new UDPInput(9621),
        new DMXOutput(1));
    hyp->addPipe(pipe);
  }

  hyp->start(Hyperion::minimal);

  while (1)
  {
    updateLed(hyp);
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
  //If receiving data, flash fast
  for(auto pipe: hyp->getPipes()){
    if(pipe->getNumPixels() > 0){
      bool flashFast = Utils::millis() % 100 < 50;
      gpio_set_level(STATUS_LED_PIN, flashFast);
      return;
    }
  }

  //If connecting or waiting for IP, flash slow
  if (Ethernet::isConnecting())
  {
    bool flashSlow = Utils::millis() % 1000 < 500;
    gpio_set_level(STATUS_LED_PIN, flashSlow);
    return;
  }

  //If connected and on standby, turn on
  if (Ethernet::isConnected())
  {
    gpio_set_level(STATUS_LED_PIN, 1);
    return;
  }

  //Status led is off by default
  gpio_set_level(STATUS_LED_PIN, 0);

  Thread::sleep(100);
}