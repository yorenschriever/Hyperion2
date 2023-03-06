#include "core/hyperion.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/thread.hpp"

#include "core/distribution/pipes/pipe.hpp"
#include "core/distribution/inputs/udpInput.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"

#include "colours.h"
#include "freakMap.h"

int main()
{
  auto hyp = new Hyperion();

  Socket s_out = Socket();
  //Socket s_in = Socket(4446);
  //char buf[500];

  auto pipe = new Pipe(
    new UDPInput(4445),
    //new UDPOutput("localhost",4446,60)
    new MonitorOutput(freakMap)
  );

  hyp->addPipe(pipe);

  hyp->start();

  while (1)
  {
    Thread::sleep(200);

    // int ret = s_in.recv(&buf, sizeof(buf));
    // if (ret)
    // {
    //   Log::info("TEST", "received package: %s", buf);
    // }

    RGBA buf2 = Hue(Utils::millis()*10);
    auto dest = IPAddress::fromIPString("127.0.0.1");
    s_out.send(&dest, 4445, (uint8_t *)&buf2, sizeof(buf2));
  }
}