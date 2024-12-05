#include "core/hyperion.hpp"
#include "distribution/inputs/fallbackInput.hpp"
#include "pattern.hpp"

LUT *NeopixelLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 255);

const int channelCount = 8;

//number of leds on each output
const int sizes[channelCount] = {200,200,200,200,200,150,150,200};

int main()
{
  auto hyp = new Hyperion();

  Network::setHostName("hyperslave4");

  hyp->hub.getParams(0)->palette->primary = RGB(255, 100, 0);
  hyp->hub.getParams(0)->palette->secondary = RGB(100, 100, 75);
  hyp->hub.getParams(0)->velocity = 0.3;

    for (int i = 0; i < channelCount; i++)
    {
      hyp->addPipe(
        new ConvertPipe<RGB,GRB>(
          new FallbackInput(
            new UDPInput(9611 + i),
            new PatternInput<RGB>(sizes[i], new GlowPulsePattern())),
          new NeopixelOutput(1 + i),
          NeopixelLut
        ));
    }

  hyp->start(Hyperion::minimal);

  while (1) Thread::sleep(1000);
}