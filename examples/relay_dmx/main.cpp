#include "hyperion.hpp"

int main()
{
  auto hyp = new Hyperion();

  Network::setHostName("hyperslave6");

  hyp->createChain(
      new UDPInput(9619),
      new DMXOutput(1));

  hyp->start(Hyperion::minimal);

  while (1)
    Thread::sleep(1000);
}
