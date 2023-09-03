#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/generation/pixelMapSplitter.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/patternCycleInput.hpp"
#include "mapping/sunMap.hpp"
#include "mapping/sunMap3d.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include <vector>

auto pSunMap = sunMap.toPolarRotate90();

std::vector<PaletteColumn::Palette> palettes = std::vector<PaletteColumn::Palette>(
    {tunnel,
     heatmap2,
     pinkSunset,
     retro,
     blueOrange,
     sunset8,
     sunset2,
     campfire,
     sunset6,
     plumBath,
     greatBarrierReef,
     redSalvation,

     sunset1,
     sunset3,
     sunset4,
     purpleGreen});

int main()
{
    auto hyp = new Hyperion();

    Tempo::AddSource(new ConstantTempo(120));

    auto input = new PatternCycleInput<RGBA>(
        sunMap.size(),
        {
            new Patterns::GrowingCirclesPattern(&sunMap),
            new Patterns::RadialGlitterFadePattern(&pSunMap),
            new Patterns::XY(&sunMap),
            new Patterns::Lighthouse(&pSunMap),
            new Patterns::GlowPulsePattern(),
            new Patterns::AngularFadePattern(&pSunMap),
            new Patterns::RadialFadePattern(&pSunMap),
            new Patterns::ChevronsConePattern(&pSunMap),
        },

        0.1 * 60 * 1000);

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        // new MonitorOutput(&hyp->webServer, sunMap)
        new MonitorOutput3d(&hyp->webServer, sunMap3d)));

    hyp->start();

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
        Thread::sleep(0.15 * 60 * 1000);
    }
}
