#include "hyperion.hpp"
#include "common/patterns/patterns-monochrome.hpp"
#include "common/patterns/patterns-led.hpp"

Hyperion hyp;
Combine dmxCombine;

int main()
{
  hyp.createChain(&dmxCombine,new DMXOutput(0));

  hyp.createChain(
    new PatternInput<Monochrome>(10, new MonochromePatterns::LFOPattern<SinFast>()), 
    dmxCombine.atDmxChannel(1)
  );

  hyp.createChain(
    new PatternInput<RGBA>(3, new LedPatterns::GlowPulsePattern()), 
    new ColorConverter<RGBA, RGB>(),
    dmxCombine.atDmxChannel(16)
  );

  //also have a look at library/common/dmxAndMonitor.hpp

  hyp.start();

  while (true)
    Thread::sleep(10);
}