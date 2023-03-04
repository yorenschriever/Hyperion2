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
#include "map.hpp"

#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/generation/patterns/mappedPatterns.h"

int main()
{

  auto lengths = std::vector<int>({
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA),
        480 * sizeof(RGBA)
      });

  auto split = InputSplitter(
      new PatternInput<RGBA>(
        map.size(),
        //new Mapped::HorizontalWavePattern<SinFast>(map,1)
        new Mapped::ConcentricWavePattern<SinFast>(map,1)
        //new Mapped::HorizontalGradientPattern(map)
      ),
      lengths, true
  );

  auto splitMap = PixelMapSplitter(
    &map, {480,480,480,480,480,480}
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

  hyp.setup();
  
  while (1)
  {
    hyp.run();
  }
}

