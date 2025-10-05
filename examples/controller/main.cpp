#include "generation/patterns/mappedPatterns.h"
#include "hyperion.hpp"
#include "ledsterMap.hpp"

int main()
{
  auto hyp = new Hyperion();

  hyp->createChain(
    new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          {
            {.column = 0, .slot = 0, .pattern = new Mapped::ConcentricWavePattern<SinFast>(&ledsterMap)},
            {.column = 0, .slot = 1, .pattern = new Mapped::HorizontalGradientPattern(&ledsterMap)},
          }),
    new ConvertColor<RGBA, RGB>(),
    new MonitorOutput(&hyp->webServer,&ledsterMap));

  hyp->start();

  while (1)
    Thread::sleep(1000);
}


