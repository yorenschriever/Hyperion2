#include "core/hyperion.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/thread.hpp"

#include "core/distribution/pipes/pipe.hpp"
#include "core/distribution/inputs/udpInput.hpp"
#include "core/distribution/outputs/udpOutput.hpp"

int main()
{
  auto hyp = Hyperion();

  Socket s_out = Socket();
  Socket s_in = Socket(4446);
  char buf[500];

  auto pipe = new Pipe(
    new UDPInput(4445),
    new UDPOutput("localhost",4446,60)
  );

  hyp.addPipe(pipe);

  hyp.setup();

  while (1)
  {
    hyp.run();
    Thread::sleep(200);

    int ret = s_in.recv(&buf, sizeof(buf));
    if (ret)
    {
      Log::info("TEST", "received package: %s", buf);
    }

    const char *buf2 = "hello";
    s_out.send(IPAddress::fromIPString("127.0.0.1"), 4445, (uint8_t *)buf2, sizeof(buf2));
  }
}