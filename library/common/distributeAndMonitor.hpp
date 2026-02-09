#pragma once
#include "hyperion.hpp"

typedef struct Slave
{
    const char *host;
    unsigned short port;
    int size; // number of lights
    IConverter* converter = nullptr;
} Slave;

typedef std::vector<Slave> Distribution;

template <class T_INPUT_COLOR = RGBA>
std::vector<Slicer::Slice> createSlices(
    int inputLength,
    Distribution slaves
    )
{
    std::vector<Slicer::Slice> slices;
    int start = 0;
    for(auto slave : slaves){
        slices.push_back({ 
            int( start * sizeof(T_INPUT_COLOR)), 
            int( slave.size * sizeof(T_INPUT_COLOR)), 
            true
        });
        start += slave.size;
    }

    slices.push_back({0, int( inputLength * sizeof(T_INPUT_COLOR)), false});
 
    if (start != inputLength){
        Log::error("DistributeAndMonitor","createSliceAndMonitorPipes: Total length of slaves (%d) does not equal number of lights in the input (%d).",start,inputLength);
    }

    return slices;
}

template <class T_OUTPUT_COLOR = GRB, class T_INPUT_COLOR = RGBA>
void distribute(
    Hyperion *hyp,
    Distribution slaves,
    Slicer *splitInput,
    LUT *lut = nullptr)
{
    auto defaultConverter = new ConvertColor<T_INPUT_COLOR, T_OUTPUT_COLOR>(lut);

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        hyp->createChain(
            splitInput->getSlice(i),
            slaves[i].converter ? slaves[i].converter : defaultConverter,
            new UDPOutput(slaves[i].host, slaves[i].port, 60)
        );
    }
}

template <class T_OUTPUT_COLOR = GRB, class T_INPUT_COLOR = RGBA>
void distributeAndMonitor(
    Hyperion *hyp,
    ControlHubInput<T_INPUT_COLOR> *input,
    PixelMap *pixelMap,
    Distribution slaves,
    LUT *lut = nullptr,
    float monitorDotSize=0.01)
{
    auto slices = createSlices<T_INPUT_COLOR>(input->length(), slaves);
    auto splitInput = new Slicer(slices);
    distribute<T_OUTPUT_COLOR, T_INPUT_COLOR>(hyp, slaves, splitInput, lut);

    hyp->createChain(input,splitInput);
    hyp->createChain(
        splitInput->getSlice(slices.size()-1),
        new ConvertColor<T_INPUT_COLOR, RGB>(),
        new MonitorOutput(&hyp->webServer, pixelMap, 60, monitorDotSize)
    );
}

template <class T_OUTPUT_COLOR = GRB, class T_INPUT_COLOR = RGBA>
void distributeAndMonitor3d(
    Hyperion *hyp,
    ControlHubInput<T_INPUT_COLOR> *input,
    PixelMap3d *pixelMap,
    Distribution slaves,
    LUT *lut = nullptr,
    float monitorDotSize=0.01)
{
    auto slices = createSlices<T_INPUT_COLOR>(input->length(), slaves);
    auto splitInput = new Slicer(slices);
    distribute<T_OUTPUT_COLOR, T_INPUT_COLOR>(hyp, slaves, splitInput, lut);

    hyp->createChain(input,splitInput);
    hyp->createChain(
        splitInput->getSlice(slices.size()-1),
        new ConvertColor<T_INPUT_COLOR, RGB>(),
        new MonitorOutput3d(&hyp->webServer, pixelMap, 60, monitorDotSize)
    );
}

