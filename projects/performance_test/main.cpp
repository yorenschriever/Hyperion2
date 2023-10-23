#include "core/hyperion.hpp"

#include "colours.h"
#include "distribution/inputs/patternInput.hpp"
#include "distribution/outputs/neopixelOutput.hpp"
#include "distribution/pipes/convertPipe.hpp"
#include "generation/patterns/pattern.hpp"
#include "utils.hpp"

class RainbowPattern : public Pattern<RGB>
{
  inline void Calculate(RGB *pixels, int width, bool firstFrame, Params*) override
  {
    for (int index = 0; index < width; index++)
    {
      pixels[index] = RGBA(Hue(0xFF * index / width + (Utils::millis() / 10))) *0.25;
    }
  }
};

int main()
{
  auto hyp = new Hyperion();

  for (int i = 1; i <= 8; i++)
  {
    auto pipe = new ConvertPipe<RGB, GRB>(
        new PatternInput<RGB>(12*60, new RainbowPattern()),
        new UDPOutput("hyperion.local",9610+i,45));
        //new NeopixelOutput(i));
    hyp->addPipe(pipe);
  }

  hyp->start();

  while (1) Thread::sleep(1000);
}
