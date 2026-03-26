#pragma once
#include "hyperion.hpp"

typedef struct Slave
{
    const char *host;
    unsigned short port;
    int size; // number of lights
    IProcessor* converter = nullptr;
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
    LUT *lut = nullptr,
    NeoPixels::Timing timing = NeoPixels::Kpbs800)
{
    auto defaultConverter = new ColorConverter<T_INPUT_COLOR, T_OUTPUT_COLOR>(lut);

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        hyp->createChain(
            splitInput->getSlice(i),
            {
                slaves[i].converter ? slaves[i].converter : defaultConverter,
                new Throttle(timing),
                //TODO if you provide a custom converter, the size of the output color might be different, leading to an incorrect led count in the analytics
                new Analytics(slaves[i].host + std::string(":") + std::to_string(slaves[i].port), sizeof(T_OUTPUT_COLOR)),
            },
            new UDPOutput(slaves[i].host, slaves[i].port, 120)
        );
    }
}

template <class T_OUTPUT_COLOR = GRB, class T_INPUT_COLOR = RGBA, class T_PixelMap = PixelMap>
void distributeAndMonitor(
    Hyperion *hyp,
    ControlHubInput<T_INPUT_COLOR> *input,
    T_PixelMap *pixelMap,
    Distribution slaves,
    LUT *lut = nullptr,
    float monitorDotSize=0.01,
    NeoPixels::Timing timing = NeoPixels::Kpbs800
    )
{
    auto slices = createSlices<T_INPUT_COLOR>(input->length(), slaves);
    auto splitInput = new Slicer(slices);
    hyp->createChain(input,splitInput);
    distribute<T_OUTPUT_COLOR, T_INPUT_COLOR>(hyp, slaves, splitInput, lut, timing);
    hyp->createChain(
        splitInput->getSlice(slices.size()-1),
        new ColorConverter<T_INPUT_COLOR, RGB>(),
        new MonitorOutput(&hyp->webServer, pixelMap, nullptr,  60, monitorDotSize)
    );
}