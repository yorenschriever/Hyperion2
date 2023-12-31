#include "colours.h"
#include "distribution/inputs/controlHubInput.hpp"
#include "distribution/inputs/websocketInput.hpp"
#include "distribution/outputs/monitorOutput.hpp"
#include "distribution/pipes/convertPipe.hpp"
#include "generation/patterns/mappedPatterns.h"
#include "hyperion.hpp"
#include "ledsterMap.hpp"

/*
This project detects pixel positions with a camera.

Load this project onto a hyper that is connected to a string of leds,
then navigate to
https://pixelmapper.local/pixelmapper/

click 'add websocket device'
- hostname: pixelmapper.local
- pixel count: the number of leds you connected
click 'Capture>>'

In the last step, you can export a .hpp file with a pixelMap that you 
can use in your other projects.

more info:
https://github.com/yorenschriever/PixelMapper

*/

int main()
{
  auto hyp = new Hyperion();
  //You need to be in a secure environment to access the camera in your browser.
  //The server needs to be setup as https
  hyp->useHttps();
  Network::setHostName("pixelmapper");

  auto pipe = new ConvertPipe<Monochrome, RGB>(
      new WebsocketInput(&hyp->webServer,"/ws/map",true),
      new NeopixelOutput(1)
      );

  hyp->addPipe(pipe);
  hyp->start();

  while (1)
    Thread::sleep(1000);
}


