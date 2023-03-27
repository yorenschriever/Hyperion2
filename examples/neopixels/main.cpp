#include "core/hyperion.hpp"

#include "colours.h"
#include "distribution/inputs/patternInput.hpp"
#include "distribution/outputs/neopixelOutput.hpp"
#include "distribution/pipes/convertPipe.hpp"
#include "generation/patterns/pattern.hpp"
#include "utils.hpp"

//This reaches 48fps for 500 leds on 8 channels (= 4000 leds)
class RainbowPattern : public Pattern<RGB>
{
  inline void Calculate(RGB *pixels, int width, bool firstFrame, Params*) override
  {
    for (int index = 0; index < width; index++)
    {
      pixels[index] = Hue(0xFF * index / width + (Utils::millis() / 10)) ;
    }
  }
};

//This empty pattern reaches 64fps for 500 leds on 8 channels (= 4000 leds)
class EmptyPattern : public Pattern<RGB>
{
  inline void Calculate(RGB *pixels, int width, bool firstFrame, Params*) override{}
};

int main()
{
  auto hyp = new Hyperion();

  for (int i = 1; i <= 8; i++)
  {
    auto pipe = new ConvertPipe<RGB, GRB>(
        new PatternInput<RGB>(500, new RainbowPattern()),
        new NeopixelOutput(i));
    hyp->addPipe(pipe);
  }

  hyp->start();

  while (1) Thread::sleep(1000);
}
