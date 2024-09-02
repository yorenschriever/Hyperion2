#include "core/hyperion.hpp"

/**
 * This example creates a stand alone hyper node that takes DMX input and distributes it
 * to its neopixel outputs.
 *
 * Neopixel output 1 will show DMX channels 1-512
 * Neopixel output 2 will show DMX channels 65-512
 * Neopixel output 3 will show DMX channels 129-512
 * Neopixel output 4 will show DMX channels 193-512
 * Neopixel output 5 will show DMX channels 257-512
 * Neopixel output 6 will show DMX channels 321-512
 * Neopixel output 7 will show DMX channels 385-512
 * Neopixel output 8 will show DMX channels 449-512
 *
 * This setup gives you some flexibility:
 * - You can make one long string with 512 dmx channels by using Neopixel output 1 only
 * - You can make 2 strings with 256 dmx channels by using Neopixel outputs 1 and 5
 * - You can make 8 strings with 64 dmx channels by using all Neopixel outputs
 * etc..
 *
 * Color space encoding will be passed as is. So if you connect an RGB led strip,
 * channel 1 will be R, 2 will be G and 3 will be B.
 */

int main()
{
  auto hyp = new Hyperion();

  for (int i = 0; i < 8; i++)
  {
    hyp->addPipe(new Pipe(
        new DMXInput(1 + i * 64),
        new NeopixelOutput(i + 1)));
  }

  hyp->start(Hyperion::minimal);

  while (1)
    Thread::sleep(1000);
}
