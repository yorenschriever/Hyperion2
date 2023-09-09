#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/outputs/cloneOutput.hpp"
#include "mapping/sunMap.hpp"
#include "mapping/sunMap3d.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include <vector>

auto pSunMap = sunMap.toPolarRotate90();

void addPaletteColumn(Hyperion *hyp);
void addSunPipe(Hyperion *hyp);

int main()
{
    auto hyp = new Hyperion();

    addPaletteColumn(hyp);
    addSunPipe(hyp);
    Tempo::AddSource(new ConstantTempo(120));

    // select first palette
    hyp->hub.buttonPressed(0, 0);
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

    hyp->hub.buttonPressed(1, 0);
    hyp->hub.setFlashColumn(1, false, true);

    hyp->hub.setColumnName(0, "Palette");
    hyp->hub.setColumnName(1, "Sun");

    hyp->start();


    while (1) Thread::sleep(60 * 1000);
}

void addSunPipe(Hyperion *hyp)
{
    auto input = new ControlHubInput<RGBA>(
        sunMap.size(),
        &hyp->hub,
        1,
        {
            new Patterns::GlitchPattern(180),
            new Patterns::GlitchPattern(60),
            new Patterns::GlitchPattern(20),
            new Patterns::RibbenClivePattern<NegativeCosFast>(),
            new Patterns::RibbenClivePattern<NegativeCosFast>(3*60),
            new Patterns::SegmentChasePattern(),
            new Patterns::SegmentChasePattern(3*60),
            new Patterns::RibbenFlashPattern(),
            new Patterns::RibbenFlashPattern(3*60),
            new Patterns::PixelGlitchPattern(),
            new Patterns::HaloOrSwirl(),
            new Patterns::HaloOrSwirl(0),
            new Patterns::HaloOrSwirl(1),
            new Patterns::Skirt(&pSunMap),
            new Patterns::GrowingCirclesPattern(&sunMap),
            new Patterns::RadialGlitterFadePattern(&pSunMap),
            new Patterns::XY(&sunMap),
            new Patterns::Lighthouse(&pSunMap),
            new Patterns::GlowPulsePattern(),
            new Patterns::AngularFadePattern(&pSunMap),
            new Patterns::RadialFadePattern(&pSunMap),
            new Patterns::ChevronsConePattern(&pSunMap),
        }
        );

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new CloneOutput({
            new MonitorOutput(&hyp->webServer, &sunMap),
            new MonitorOutput3d(&hyp->webServer, &sunMap3d)
        })
    ));
}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        0,
        {
            campfire,
            pinkSunset,
            sunset8,
            heatmap,
            heatmap2,
            sunset2,
            retro,
            tunnel,

            sunset6,
            sunset7,
            sunset1,
            coralTeal,
            deepBlueOcean,
            redSalvation,
            plumBath,
            sunset4,
            candy,
            sunset3,
            greatBarrierReef,
            blueOrange,
            peach,
            denseWater,
            purpleGreen,
            sunset5,
            salmonOnIce,
        });
    hyp->hub.subscribe(paletteColumn);
}