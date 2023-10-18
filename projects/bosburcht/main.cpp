#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/outputs/cloneOutput.hpp"
#include "mapping/windowMap.hpp"
#include "mapping/windowMap3dCombined.hpp"
#include "mapping/ceilingMap.hpp"
#include "mapping/ceilingMap3dCombined.hpp"
#include "mapping/chandelierMap3d.hpp"
#include "mapping/chandelierMap3dCombined.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include <vector>
#include "setViewParams.hpp"

// auto pWindowMap = windowMap.toPolarRotate90();
// auto pCeilingMap = ceilingMap.toPolarRotate90();
// // auto pChandelierMap = chandelierMap.toPolarRotate90();

// auto cMap = ceilingMap;
// auto pMap = pCeilingMap;

// auto cMap = chandelierMap;
// auto pMap = pChandelierMap;

void addPaletteColumn(Hyperion *hyp);

void addWindowPipe(Hyperion *);
void addChandelierPipe(Hyperion *);
void addCeilingPipe(Hyperion *);

int main()
{
    auto hyp = new Hyperion();

    hyp->hub.addParams(new Params("second params"));

    addPaletteColumn(hyp);
    
    addWindowPipe(hyp);
    addChandelierPipe(hyp);
    addCeilingPipe(hyp);
    
    // addCeilingPipe(hyp);
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
    setViewParams(hyp);

    while (1) Thread::sleep(60 * 1000);
}

void addWindowPipe(Hyperion *hyp)
{
    int i=0;

    auto inputL = new ControlHubInput<RGBA>(
        windowMap3dCombinedLeft.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&windowMap3dCombinedLeft)},
        }
        );

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        inputL,
        new CloneOutput({
            new MonitorOutput3d(&hyp->webServer, &windowMap3dCombinedLeft),
            //new MonitorOutput3d(&hyp->webServer, &sunMap3d)
        })
    ));

    i=0;
    auto inputR = new ControlHubInput<RGBA>(
        windowMap3dCombinedRight.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&windowMap3dCombinedRight)},
        }
        );

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        inputR,
        new CloneOutput({
            new MonitorOutput3d(&hyp->webServer, &windowMap3dCombinedRight),
            //new MonitorOutput3d(&hyp->webServer, &sunMap3d)
        })
    ));
}

void addChandelierPipe(Hyperion *hyp)
{
    int i=0;

    auto input = new ControlHubInput<RGBA>(
        chandelierMap3dCombined.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&chandelierMap3dCombined)},
        }
        );

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new CloneOutput({
            new MonitorOutput3d(&hyp->webServer, &chandelierMap3dCombined),
            //new MonitorOutput3d(&hyp->webServer, &sunMap3d)
        })
    ));
}


void addCeilingPipe(Hyperion *hyp)
{
    int i=0;

    auto input = new ControlHubInput<RGBA>(
        ceilingMap3dCombined.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&ceilingMap3dCombined)},
        }
        );

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new CloneOutput({
            new MonitorOutput3d(&hyp->webServer, &ceilingMap3dCombined),
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

