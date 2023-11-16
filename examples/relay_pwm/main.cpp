#include "core/distribution/outputs/pwmOutput.hpp"
#include "core/hyperion.hpp"

int main()
{
  auto hyp = new Hyperion();

  Network::setHostName("hyperslave7");

  hyp->addPipe(new Pipe(
      new UDPInput(9620),
      new PWMOutput()));

  hyp->start(Hyperion::minimal);

  while (1)
    Thread::sleep(1000);
}
