#include "hyperion.hpp"

std::string analyticsName(uint16_t port){
    return Network::getHostName() + std::string(".local:") + std::to_string(port);
}

int main()
{
  auto hyp = new Hyperion();

  Network::setHostName("hyperslave5");

  hyp->createChain(
      new UDPInput(9621),
      new Analytics(analyticsName(9621)),
      new SpiOutput(1,0));

  hyp->createChain(
      new UDPInput(9622),
      new Analytics(analyticsName(9622)),
      new SpiOutput(5,4));

  hyp->start(Hyperion::minimal);

  new AnalyticsUDP();

  while (1) Thread::sleep(1000);
}
