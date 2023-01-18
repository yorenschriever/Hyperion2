#include "core/hyperion.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/ethernet.hpp"

#include "platform/includes/log.hpp"

int main()
{
  auto hyp = Hyperion();
  hyp.setup();

  while (1)
  {
    hyp.run();
    Thread::sleep(250);
  }
}