#pragma once
#include "hyperion.hpp"

typedef struct
{
    const char *host;
    const unsigned short port;
    const int size; // number of lights
} Slave;

typedef std::vector<Slave> Distribution;

template <class T_INPUT_COLOUR = RGBA>
std::vector<Slicer::Slice> createSlices(
    ControlHubInput<T_INPUT_COLOUR> *input,
    Distribution slaves
    )
{
    std::vector<Slicer::Slice> slices;
    int start = 0;
    for(auto slave : slaves){
        slices.push_back({ 
            int( start * sizeof(T_INPUT_COLOUR)), 
            int( slave.size * sizeof(T_INPUT_COLOUR)), 
            true
        });
        start += slave.size;
    }

    slices.push_back({0, int( input->length() * sizeof(T_INPUT_COLOUR)), false});
 
    if (start != input->length()){
        Log::error("DistributeAndMonitor","createSliceAndMonitorPipes: Total length of slaves (%d) does not equal number of lights in the input (%d).",start,input->length());
    }

    return slices;
}

template <class T_OUTPUT_COLOUR = GRB, class T_INPUT_COLOUR = RGBA>
void distribute(
    Hyperion *hyp,
    Distribution slaves,
    Slicer *splitInput,
    LUT *lut = nullptr)
{
    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        hyp->createChain(
            splitInput->getSlice(i),
            new ConvertColor<T_INPUT_COLOUR, T_OUTPUT_COLOUR>(lut),
            new UDPOutput(slaves[i].host, slaves[i].port, 60)
        );
    }
}

template <class T_OUTPUT_COLOUR = GRB, class T_INPUT_COLOUR = RGBA>
void distributeAndMonitor(
    Hyperion *hyp,
    ControlHubInput<T_INPUT_COLOUR> *input,
    PixelMap *pixelMap,
    Distribution slaves,
    LUT *lut = nullptr,
    float monitorDotSize=0.01)
{
    auto slices = createSlices<T_INPUT_COLOUR>(input, slaves);
    auto splitInput = new Slicer(slices);
    distribute<T_OUTPUT_COLOUR, T_INPUT_COLOUR>(hyp, slaves, splitInput, lut);

    hyp->createChain(input,splitInput);
    hyp->createChain(
        splitInput->getSlice(slices.size()-1),
        new ConvertColor<T_INPUT_COLOUR, RGB>(),
        new MonitorOutput(&hyp->webServer, pixelMap, 60, monitorDotSize)
    );
}

template <class T_OUTPUT_COLOUR = GRB, class T_INPUT_COLOUR = RGBA>
void distributeAndMonitor3d(
    Hyperion *hyp,
    ControlHubInput<T_INPUT_COLOUR> *input,
    PixelMap3d *pixelMap,
    Distribution slaves,
    LUT *lut = nullptr,
    float monitorDotSize=0.01)
{
    auto slices = createSlices<T_INPUT_COLOUR>(input, slaves);
    auto splitInput = new Slicer(slices);
    distribute<T_OUTPUT_COLOUR, T_INPUT_COLOUR>(hyp, slaves, splitInput, lut);

    hyp->createChain(input,splitInput);
    hyp->createChain(
        splitInput->getSlice(slices.size()-1),
        new ConvertColor<T_INPUT_COLOUR, RGB>(),
        new MonitorOutput3d(&hyp->webServer, pixelMap, 60, monitorDotSize)
    );
}

