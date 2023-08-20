#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/patternCycleInput.hpp"
#include "mapping/ledsterMap.hpp"
#include "mapping/ledsterMap3d.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include <vector>

LUT *ledsterLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

auto cLedsterMap3d = ledsterMap3d.toCylindricalRotate90();

std::vector<PaletteColumn::Palette> palettes = std::vector<PaletteColumn::Palette>({
    pinkSunset,
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

    auto input = new PatternCycleInput<RGBA>(ledsterMap3d.size(), {
        new Patterns::Lighthouse(cLedsterMap3d),
        new Patterns::PetalChase(cLedsterMap3d),
        new Patterns::SnowflakePatternLedster(),
        new Patterns::SnakePattern(),
        new Patterns::XY(ledsterMap3d),
        new Patterns::RibbenClivePattern<Glow>(10000, 1, 0.15),
        new Patterns::GrowingCirclesPattern(ledsterMap3d),
        new Patterns::SpiralPattern(cLedsterMap3d),
        new Patterns::SegmentChasePattern(),
        new Patterns::GlowPulsePattern(),
        new Patterns::PetalRotatePattern(),
    },
    3 * 60 * 1000);

#if ESP_PLATFORM
    hyp->addPipe(new ConvertPipe<RGBA, GRB>(
        input,
        new NeopixelOutput(1),
        //new SpiOutput(0,1),
        ledsterLut));
    hyp->start(minimal);
#else
    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new MonitorOutput(&hyp->webServer, ledsterMap)));
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
        Thread::sleep(4 * 60 * 1000);
    }
}
