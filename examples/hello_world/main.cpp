#include "platform/includes/thread.hpp"
#include "platform/includes/log.hpp"

int main()
{
  Log::info("HYP", "initializing");

  while (1)
  {
    Log::info("HYP", "running");
    Thread::sleep(250);
  }
}