#include "hyperion.hpp"
#include "pattern.hpp"

// This example shows how to use a spatial grid to efficiently find pixels within 
//a certain range of a position. This is useful when working with particle systems.
int main()
{
    auto hyp = new Hyperion();

    auto map = gridMap(100, 100);

    hyp->createChain(
        new ControlHubInput<RGBA>(map.size(), &hyp->hub, {
            {.column = 0, .slot = 0, .pattern = new BeerBubblesPattern(&map)},
        }),
        {new ColorConverter<RGBA, RGB>(),
        new Analytics("bubblePattern")},
        new MonitorOutput(&hyp->webServer, &map, nullptr, 120, 0.01)
    );

    hyp->hub.setSlotActive(0, 0, true);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
