#include "core/hyperion.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/ipAddress.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/thread.hpp"

#include "core/distribution/pipes/convertPipe.hpp"
#include "core/distribution/inputs/udpInput.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"

#include "colours.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "mapping/ledsterMap.hpp"
#include "mapping/columnMap.hpp"

#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/generation/patterns/mappedPatterns.h"

int main()
{

  Params::primaryColour = RGB(255,0,255);
  Params::secondaryColour = RGB(50,50,50);

  auto lengths = std::vector<int>();

  auto split = InputSplitter(
      new PatternInput<RGBA>(
        columnMap.size(),
        new Mapped::ConcentricWavePattern<SinFast>(columnMap,2,2)
      ),
      {
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA)
      }, 
      true
  );

  auto splitMap = PixelMapSplitter(
    &columnMap, {480,480,480,480,480,480}
  );

  auto hyp = Hyperion();

  for (int i=0;i<split.size();i++)
  {
    auto pipe = new ConvertPipe<RGBA,RGB>(
      split.getInput(i),
      new MonitorOutput(splitMap.getMap(i))
    );
    hyp.addPipe(pipe);
  }

  auto ledsterPipe = new ConvertPipe<RGBA,RGB>(
    new PatternInput<RGBA>(
      ledsterMap.size(),
      new Mapped::ConcentricWavePattern<SinFast>(ledsterMap,2,2)
    ),
    new MonitorOutput(ledsterMap)
  );
  hyp.addPipe(ledsterPipe);

  hyp.setup();
  
  while (1)
  {
    hyp.run();
  }
}

