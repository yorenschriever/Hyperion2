#include "../../../main.cpp"
#include "thread.hpp"

extern "C"
{
  void app_main(void);
}

void app_main(void)
{
  main();
  Thread::destroy();
}