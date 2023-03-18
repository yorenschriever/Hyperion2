#include "core/hyperion.hpp"

#include "colours.h"
#include "distribution/inputs/patternInput.hpp"
#include "distribution/outputs/neopixelOutput.hpp"
#include "distribution/pipes/convertPipe.hpp"
#include "generation/patterns/pattern.hpp"
#include "utils.hpp"

#include <iostream>
#include <cstdlib>

int main()
{
  auto hyp = new Hyperion();

  // for (int i = 1; i <= 8; i++)
  // {
  //   auto pipe = new ConvertPipe<RGB, GRB>(
  //       new PatternInput<RGB>(500, new RainbowPattern()),
  //       new NeopixelOutput(i));
  //   hyp->addPipe(pipe);
  // }

  hyp->start();

  while (1) Thread::sleep(1000);
}
