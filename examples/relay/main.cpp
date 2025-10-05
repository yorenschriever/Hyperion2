#include "hyperion.hpp"

// This example creates a node that listens to UDP ports 9611 - 9618,
// and puts their data on the neopixel outputs 1-8
// This is useful when you have large led installations and want to
// distribute the data over ethernet to multiple relays

int main()
{
  auto hyp = new Hyperion();

  Network::setHostName("hyperslave4");

  for (int i = 1; i <= 8; i++)
  {
    hyp->createChain(
        new UDPInput(9610 + i),
        new NeopixelOutput(i));
  }

  hyp->start(Hyperion::minimal);

  while (1) Thread::sleep(1000);
}
