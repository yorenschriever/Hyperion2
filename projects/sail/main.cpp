#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/patternCycleInput.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include "sailMap.hpp"
#include <vector>

LUT *ledsterLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

auto pSailMap = sailMap.toPolarRotate90();

std::vector<PaletteColumn::Palette> palettes = std::vector<PaletteColumn::Palette>(
    {pinkSunset,
     sunset8,
     heatmap2,
     sunset2,
     sunset6,
     plumBath,
     redSalvation});

const Hyperion::Config minimal = {
    .network = false,
    .rotary = false,
    .display = false,
    .midi = false,
    .tempo = false,
    .web = false,
};

int main()
{
    auto hyp = new Hyperion();

    Tempo::AddSource(new ConstantTempo(120));

    auto input = new PatternCycleInput<RGBA>(
        sailMap.size(),
        {
            new Patterns::AngularFadePattern(pSailMap), 
            new Patterns::Lighthouse(pSailMap),
            new Patterns::RadialFadePattern(pSailMap), 
            new Patterns::XY(sailMap),  
            new Patterns::GrowingCirclesPattern(sailMap),
            new Patterns::ChevronsConePattern(pSailMap),   
            new Patterns::ChevronsPattern(sailMap),   
            new Patterns::GrowingCirclesPattern2(sailMap),
            new Patterns::RadialGlitterFadePattern(pSailMap), 
            new Patterns::SpiralPattern(pSailMap),
            new Patterns::GlowPulsePattern(),
        },
        0.1 * 60 * 1000);

#if ESP_PLATFORM
    int splitSize = sailMap.size() / 2 * sizeof(RGBA);
    auto splitInput = new InputSplitter(input, {splitSize, splitSize}, true);

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(0),
        new SpiOutput(4, 5),
        ledsterLut));
    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(1),
        new SpiOutput(0, 1),
        ledsterLut));

    hyp->start(minimal);
#else
    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new MonitorOutput(&hyp->webServer, sailMap)));

    hyp->start();
#endif

    int paletteColumnIndex = 0;
    while (1)
    {
        input->params.gradient = palettes[paletteColumnIndex].gradient;
        input->params.primaryColour = palettes[paletteColumnIndex].primary;
        input->params.secondaryColour = palettes[paletteColumnIndex].secondary;
        input->params.highlightColour = palettes[paletteColumnIndex].highlight;

        paletteColumnIndex++;
        if (paletteColumnIndex >= palettes.size())
            paletteColumnIndex = 0;
        Thread::sleep(0.1 * 1.5 * 60 * 1000);
    }
}
