#include "core/distribution/inputs/inputSlicer.hpp"
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
#include "core/distribution/luts/colourCorrectionLut.hpp"

LUT *columnsLut = nullptr; //new ColourCorrectionLUT(1, 255, 200, 200, 200);

auto cwindowMap3dCombinedLeft = windowMap3dCombinedLeft.toCylindricalRotate90();
auto cwindowMap3dCombinedRight = windowMap3dCombinedRight.toCylindricalRotate90();
auto cchandelierMap3dCombined = chandelierMap3dCombined.toCylindricalRotate90();
auto cceilingMap3dCombined = ceilingMap3dCombined.toCylindricalRotate90();
auto cchandelierMap3d = chandelierMap3d.toCylindricalRotate90();

void addPaletteColumn(Hyperion *hyp);

void addWindowPipe(Hyperion *);
void addChandelierPipe(Hyperion *);
void addCeilingPipe(Hyperion *);

int main()
{
    auto hyp = new Hyperion();

    // hyp->hub.addParams(new Params("second params"));

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
    // hyp->hub.setFlashColumn(1, false, true);

    hyp->hub.setColumnName(0, "Palette");
    hyp->hub.setColumnName(1, "Test");


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
            {.column = 1, .slot = i++, .pattern=new Patterns::GlowPulsePattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cwindowMap3dCombinedLeft)},
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
            {.column = 1, .slot = i++, .pattern=new Patterns::GlowPulsePattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cwindowMap3dCombinedRight)},
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
    int nleds = chandelierMap3dCombined.size();
    int nbytes = nleds * sizeof(RGBA);
    int i=0;

    auto input = new ControlHubInput<RGBA>(
        nleds,
        &hyp->hub,
        {
            {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&chandelierMap3dCombined)},
            {.column = 1, .slot = i++, .pattern=new Patterns::GlowPulsePattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cchandelierMap3dCombined)},
            {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cchandelierMap3d)},
        }
        );

    auto splitInput = new InputSlicer(input,
    {
        {0* nbytes/8, nbytes/8, true },
        {1* nbytes/8, nbytes/8, true },
        {2* nbytes/8, nbytes/8, true },
        {3* nbytes/8, nbytes/8, true },
        {4* nbytes/8, nbytes/8, true },
        {5* nbytes/8, nbytes/8, true },
        {6* nbytes/8, nbytes/8, true },
        {7* nbytes/8, nbytes/8, true },
        {0,nbytes, false},
    });

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        auto pipe = new ConvertPipe<RGBA, BGR>(
            splitInput->getInput(i),
            new UDPOutput("hyperion.local", 9611+i, 60),
            columnsLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 1),
            new MonitorOutput3d(&hyp->webServer,&chandelierMap3dCombined)));
}


void addCeilingPipe(Hyperion *hyp)
{
    int i=0;

    auto input = new ControlHubInput<RGBA>(
        ceilingMap3dCombined.size(),
        &hyp->hub,
        {
            {.column = 1, .slot = i++, .pattern=new Patterns::Quadrants3d(&ceilingMap3dCombined)},
            {.column = 1, .slot = i++, .pattern=new Patterns::GlowPulsePattern()},
            {.column = 1, .slot = i++, .pattern=new Patterns::Lighthouse(&cceilingMap3dCombined)},
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

