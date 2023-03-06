#include "colours.h"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/inputs/udpInput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/nullOutput.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/generation/patterns/mappedPatterns.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/hyperion.hpp"
#include "mapping/columnMap.hpp"
#include "mapping/ledsterMap.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/thread.hpp"

void addLedsterPipe(Hyperion *hyp);
void addColumnPipes(Hyperion *hyp);

int main()
{
  Params::primaryColour = RGB(255, 0, 255);
  Params::secondaryColour = RGB(50, 50, 50);

  auto hyp = new Hyperion();

  addColumnPipes(hyp);
  addLedsterPipe(hyp);

  hyp->start();
  while (1) Thread::sleep(1000);
}

void addLedsterPipe(Hyperion *hyp)
{
  auto ledsterPipe = new ConvertPipe<RGBA, RGB>(
      new PatternInput<RGBA>(
          ledsterMap.size(),
          new Mapped::ConcentricWavePattern<SinFast>(ledsterMap, 2, 2)),
      new MonitorOutput(ledsterMap));
  hyp->addPipe(ledsterPipe);
}

void addColumnPipes(Hyperion *hyp)
{
  // Generate 1 pattern, and split it up in six outputs,
  // because UDPOutput (and therefore MonitorOutput) are limited by a
  // maximum transfer size of 2*1440 bytes
  auto splitInput = new InputSplitter(
      new PatternInput<RGBA>(
          columnMap.size(),
          new Mapped::ConcentricWavePattern<SinFast>(columnMap, 2, 2)),
      {480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA),
       480 * sizeof(RGBA)},
      true);

  auto splitMap = PixelMapSplitter(
      &columnMap, {480, 480, 480, 480, 480, 480});

  for (int i = 0; i < splitInput->size(); i++)
  {
    auto pipe = new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(i),
        new MonitorOutput(splitMap.getMap(i)));
    hyp->addPipe(pipe);
  }
}