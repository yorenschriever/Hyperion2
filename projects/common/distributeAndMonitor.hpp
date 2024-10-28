#include "core/hyperion.hpp"

typedef struct
{
    const char *host;
    const unsigned short port;
    const int size; // number of lights
} Slave;

template <class T_OUTPUT_COLOUR = GRB, class T_INPUT_COLOUR = RGBA>
void distributeAndMonitor(
    Hyperion *hyp,
    ControlHubInput<T_INPUT_COLOUR> *input,
    PixelMap *pixelMap,
    std::vector<Slave> slaves,
    LUT *lut = nullptr,
    float monitorDotSize=0.01)
{
    std::vector<InputSlicer::Slice> slices;
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
        Log::error("COMMON","createSliceAndMonitorPipes: Total length of slaves (%d) does not equal number of lights in the input (%d).",start,input->length());
    }

    auto splitInput = new InputSlicer(input,slices);

    for (int i = 0; i < slices.size() - 1; i++)
    {
        auto pipe = new ConvertPipe<T_INPUT_COLOUR, T_OUTPUT_COLOUR>(
            splitInput->getInput(i),
            new UDPOutput(slaves[i].host, slaves[i].port, 60),
            lut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<T_INPUT_COLOUR, RGB>(
            splitInput->getInput(slices.size()-1),
            new MonitorOutput(&hyp->webServer, pixelMap, 60, monitorDotSize)));
}