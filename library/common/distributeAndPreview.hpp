#pragma once
#include "hyperion.hpp"
#include "distributeAndMonitor.hpp"

using SlotPatterns = std::vector<ControlHubInput<RGBA>::SlotPattern>;
using SlotPatternsGenerator = std::function<SlotPatterns()>;

template <class T_OUTPUT_COLOR = GRB, class T_INPUT_COLOR = RGBA, class T_PixelMap = PixelMap>
void distributeAndPreview(
    Hyperion *hyp,
    SlotPatternsGenerator slotPatternsGenerator,
    T_PixelMap *pixelMap,
    Distribution slaves,
    LUT *lut = nullptr,
    float monitorDotSize=0.01,
    int activatedMask = ControlHub::ALL, //use this to show only the preview, or also the active patterns (ControlHub::ALL or ControlHub::PREVIEW)
    bool showMasterMonitor = false) //set to true is you want 2 windows: one for the monitor and one for the preview. If false, only the preview window will be shown. Use that in combination with ControlHub::ALL
{
    auto slotPatterns = slotPatternsGenerator();
    auto input = new ControlHubInput<T_INPUT_COLOR>(pixelMap->size(), &hyp->hub, slotPatterns);

    if (showMasterMonitor) {
        distributeAndMonitor<T_OUTPUT_COLOR, T_INPUT_COLOR>(
            hyp,
            input,
            pixelMap,
            slaves,
            lut,
            monitorDotSize);
    } else {
        auto slices = createSlices<T_INPUT_COLOR>(input->length(), slaves);
        auto slicedInputs = new Slicer(slices);
        hyp->createChain(input,slicedInputs);
        distribute<T_OUTPUT_COLOR, T_INPUT_COLOR>(hyp, slaves, slicedInputs, lut);
    }

    auto slotPatternsValueForPreview =  slotPatternsGenerator();
    for (auto &slotPattern : slotPatternsValueForPreview){
        slotPattern.noMasterDim = true;
        // slotPattern.paramsSlot = 1; //use a different params slot for preview, so you can preview palettes and other params (needs to have a proper ui first)
    }
    hyp->createChain(
        (new ControlHubInput<RGBA>(pixelMap->size(), &hyp->hub, slotPatternsValueForPreview))->setActivatedMask(activatedMask),
        new ColorConverter<RGBA, RGB>(),
        new MonitorOutput(&hyp->webServer,pixelMap,"Preview", 60, monitorDotSize));
}