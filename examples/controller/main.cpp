#include "colours.h"
#include "distribution/inputs/controlHubInput.hpp"
#include "distribution/outputs/monitorOutput.hpp"
#include "distribution/pipes/convertPipe.hpp"
#include "generation/patterns/mappedPatterns.h"
#include "hyperion.hpp"
#include "ledsterMap.hpp"

int main()
{
  auto hyp = new Hyperion();

  auto pipe = new ConvertPipe<RGBA, RGB>(
      new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          {
            {.column = 0, .slot = 0, .pattern = new Mapped::ConcentricWavePattern<SinFast>(ledsterMap)},
            {.column = 0, .slot = 1, .pattern = new Mapped::HorizontalGradientPattern(ledsterMap)},
          }),
      new MonitorOutput(ledsterMap));

  hyp->addPipe(pipe);

  hyp->start();

  while (1)
    Thread::sleep(1000);
}


