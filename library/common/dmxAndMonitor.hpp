#pragma once
#include "hyperion.hpp"

template <class T_DMX_COLOR = Monochrome, class T_INPUT_COLOR = Monochrome>
void DMXAndMonitor(Hyperion *hyp, ControlHubInput<T_INPUT_COLOR> *input, int size, Combine *dmxCombine, int startChannel, PixelMap *pixelMap, float monitorDotSize = 0.01, LUT *lut = nullptr)
{
    auto clone = new Slicer(
        {
            {0, size * int(sizeof(T_INPUT_COLOR)), true},
            {0, size * int(sizeof(T_INPUT_COLOR)), false},
        });
    
    hyp->createChain(input, clone);

    hyp->createChain(
        clone->getSlice(0),
        new ColorConverter<T_INPUT_COLOR, T_DMX_COLOR>(lut),
        dmxCombine->atDmxChannel(startChannel)
    );

    hyp->createChain(
        clone->getSlice(1),
        new ColorConverter<T_INPUT_COLOR, RGB>(),
        new MonitorOutput(&hyp->webServer, pixelMap, nullptr,  60, monitorDotSize)
    );
}