#include "core/hyperion.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/generation/patterns/mappedPatterns.h"

#include "colours.h"
#include "freakMap.h"

// This example shows you how you can display another device's UDP output.
// This can be useful for debugging when that device does not run a web server,
// and therefore cannot have a monitorOutput.

int main()
{
  // Use this code to listen to UDP messages and display them using the monitorInput
  auto hyp = new Hyperion();

  auto pipe = new Pipe(
      new UDPInput(4445),
      new MonitorOutput(&hyp->webServer, &freakMap));

  hyp->addPipe(pipe);
  hyp->start();

  // End of the example




  // Below you find code to generate messages. Normally this would run on the machine without a web server
  auto hyp_on_other_machine = new Hyperion();

  auto pipe_on_other_machine = new ConvertPipe<RGBA,RGB>(
      new PatternInput(freakMap.size(),new Mapped::ConcentricWavePattern<SawDown>(&freakMap)),
      new UDPOutput("localhost",4445,60)
  );

  hyp_on_other_machine->addPipe(pipe_on_other_machine);
  hyp_on_other_machine->start(Hyperion::minimal);

  while(true) Thread::sleep(10);
}