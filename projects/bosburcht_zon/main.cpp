#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/outputs/cloneOutput.hpp"
#include "mapping/sunMap.hpp"
#include "mapping/sunMap3d.hpp"
#include "core/generation/palettes.hpp"
#include "patterns.hpp"
#include <vector>

auto pSunMap = sunMap.toPolarRotate90();

void addPaletteColumn(Hyperion *hyp);
void addSunPipe(Hyperion *hyp);

int main()
{
    auto hyp = new Hyperion();

    hyp->hub.addParams(new Params("second params"));

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
    int i=0;

    auto input = new ControlHubInput<RGBA>(
        sunMap.size(),
        &hyp->hub,
        //1,
        {
            {.column = 1, .slot = i++, .pattern=new Patterns::GlitchPattern(180)},
            {.column = 1, .slot = i++, .pattern=new Patterns::GlitchPattern(60)},
            {.column = 1, .slot = i++, .pattern=new Patterns::GlitchPattern(20)},
            {.column = 1, .slot = i++, .pattern=new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = 1, .slot = i++, .pattern=new Patterns::RibbenClivePattern<NegativeCosFast>(3*60)},
            {.column = 1, .slot = i++, .pattern=new Patterns::SegmentChasePattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::SegmentChasePattern(3*60)},
            {.column = 1, .slot = i++, .pattern=new Patterns::RibbenFlashPattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::RibbenFlashPattern(3*60)},
            {.column = 1, .slot = i++, .pattern=new Patterns::PixelGlitchPattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::HaloOrSwirl()},
            {.column = 1, .slot = i++, .pattern=new Patterns::HaloOrSwirl(0)},
            {.column = 1, .slot = i++, .pattern=new Patterns::HaloOrSwirl(1)},
            {.column = 1, .slot = i++, .pattern=new Patterns::Skirt(&pSunMap)},
            {.column = 1, .slot = i++, .pattern=new Patterns::GrowingCirclesPattern(&sunMap)},
            {.column = 1, .slot = i++, .pattern=new Patterns::RadialGlitterFadePattern(&pSunMap)},
            {.column = 1, .slot = i++, .pattern=new Patterns::XY(&sunMap)},
            {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&pSunMap)},
            {.column = 1, .slot = i++, .pattern=new Patterns::GlowPulsePattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::AngularFadePattern(&pSunMap)},
            {.column = 1, .slot = i++, .pattern=new Patterns::RadialFadePattern(&pSunMap)},
            {.column = 1, .slot = i++, .pattern=new Patterns::ChevronsConePattern(&pSunMap)},

            {.column = 2, .slot = 0, .paramsSlot=1, .pattern=new Patterns::Lighthouse(&pSunMap)},
        }
        );

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new CloneOutput({
            new MonitorOutput(&hyp->webServer, &sunMap),
            //new MonitorOutput3d(&hyp->webServer, &sunMap3d)
        })
    ));


}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        0,
        0,
        {
            &campfire,
            &pinkSunset,
            &sunset8,
            &heatmap,
            &heatmap2,
            &sunset2,
            &retro,
            &tunnel,

            &sunset6,
            &sunset7,
            &sunset1,
            &coralTeal,
            &deepBlueOcean,
            &redSalvation,
            &plumBath,
            &sunset4,
            &candy,
            &sunset3,
            &greatBarrierReef,
            &blueOrange,
            &peach,
            &denseWater,
            &purpleGreen,
            &sunset5,
            &salmonOnIce,
        });
    hyp->hub.subscribe(paletteColumn);
}