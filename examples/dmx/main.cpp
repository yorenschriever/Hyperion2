#include "hyperion.hpp"
#include "common/patterns/patterns-monochrome.hpp"
Hyperion hyp;

int main()
{
  hyp.createChain(
    new PatternInput<Monochrome>(10, new MonochromePatterns::LFOPattern<SinFast>()),
    new DMXOutput(0)
  );

  hyp.start();

  while (true)
    Thread::sleep(10);
}