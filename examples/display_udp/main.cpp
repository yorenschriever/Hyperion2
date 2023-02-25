#include "core/hyperion.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/log.hpp"

int main()
{
  auto hyp = Hyperion();
  hyp.setup();

  Socket s_out = Socket();
  Socket s_in = Socket(4445);
  char buf[500];

  while (1)
  {
    hyp.run();
    Thread::sleep(50);

    int ret = s_in.recv(&buf,sizeof(buf));
    if (ret){
      Log::info("TEST","received package: %s", buf);
    }

    const char* buf2 = "hello";
    s_out.send(IPAddress::fromIPString("127.0.0.1"),4445,(uint8_t*)buf2,sizeof(buf2));
  }
}