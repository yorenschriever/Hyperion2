#include "core/hyperion.hpp"
#include "platform/includes/thread.hpp"

extern "C"
{
  void app_main(void);
}
int main()
{
  app_main();
}

void app_main(void)
{
  Log::info("HELLO_WORLD", "log working");

  auto hyp = Hyperion();
  hyp.setup();

  while (1)
  {
    hyp.run();
    Thread::sleep(250);
  }
}