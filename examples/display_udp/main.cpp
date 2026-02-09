#include "hyperion.hpp"
#include "core/generation/patterns/mappedPatterns.h"
#include "freakMap.h"

// This example shows you how you can display another device's UDP output.
// This can be useful for debugging when that device does not run a web server,
// and therefore cannot have a monitorOutput.

int main()
{
  // Use this code to listen to UDP messages and display them using the monitorInput
  auto hyp = new Hyperion();

  hyp->createChain(
      new UDPInput(4445),
      new MonitorOutput(&hyp->webServer, &freakMap));
  hyp->start();

  // End of the example




  // Below you find code to generate led data. Normally this would run on the machine without a web server
  // But for the sake of example for will run on the same machine here.
  auto hyp_on_other_machine = new Hyperion();

  hyp_on_other_machine->createChain(
    new PatternInput(freakMap.size(),new Mapped::ConcentricWavePattern<SawDown>(&freakMap)),
    new ColorConverter<RGBA, RGB>(),
    new UDPOutput("localhost",4445,60)
  );

  hyp_on_other_machine->start(Hyperion::minimal);

  while(true) Thread::sleep(10);
}