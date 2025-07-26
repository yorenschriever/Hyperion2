#include "../common/distributeAndMonitor.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/setViewParams.hpp"
#include "core/hyperion.hpp"
#include "mapping/cageMap.hpp"

void addCagePipe(Hyperion *hyp)
{

  PixelMap3d *map = &cageMap;
  PixelMap3d::Cylindrical pmap = cageMap.toCylindricalXY(0, 0);

  auto input = new ControlHubInput<RGBA>(

      map->size(),
      &hyp->hub,
      {

          // Static
          {.column = 1, .slot = 0, .pattern = new LedPatterns::PalettePattern(0, "Primary")},
          {.column = 1, .slot = 1, .pattern = new LedPatterns::PalettePattern(1, "Secondary")},

      });

  distributeAndMonitor3d<BGR, RGBA>(
      hyp, input, map,
      {
          {"hypernode1.local", 9611, 6 * 60},
          {"hypernode1.local", 9612, 6 * 60},
          {"hypernode1.local", 9613, 6 * 60},
          {"hypernode1.local", 9614, 6 * 60},
          {"hypernode1.local", 9615, 6 * 60},
          {"hypernode1.local", 9616, 6 * 60},
          {"hypernode1.local", 9617, 6 * 60},
          {"hypernode1.local", 9618, 6 * 60},
      });

  hyp->hub.setColumnName(1, "Static");
}

int main()
{
  auto hyp = new Hyperion();

  addCagePipe(hyp);

  hyp->start();
  // setViewParams(hyp, viewParamsDefault);
  // setViewParams(hyp, viewParamsTop);
  setViewParams(hyp, viewParamsFront);
  // setViewParams(hyp, viewParamsSide);

  while (1)
  {
    Thread::sleep(1000);
  }
}
