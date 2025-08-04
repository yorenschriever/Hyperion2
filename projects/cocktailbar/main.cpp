#include "MotorPosition.hpp"
#include "core/hyperion.hpp"
#include "patterns-window.hpp"

#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-monochrome.hpp"

LUT *incandescentLut = new IncandescentLUT(1.6, 255, 0);

void setupPalette(Hyperion *hyp)
{
    const int columnIndex = 0;

    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        columnIndex,
        0,
        {
            
            &pinkSunset,
            &sunset8,
            &campfire,
            &heatmap2,
            &sunset2,
            &sunset7,
            &tunnel,
            &redSalvation,
            &plumBath,
            

            &sunset6,
            
            &sunset1,
            &coralTeal,
            &deepBlueOcean,
            
            &heatmap,
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
            &sunset2,
            &retro,
        });
    hyp->hub.subscribe(paletteColumn);

    hyp->hub.buttonPressed(columnIndex, 0);
    hyp->hub.setColumnName(columnIndex, "Palette");
    hyp->hub.setFlashColumn(columnIndex, false, true);
    hyp->hub.setForcedSelection(columnIndex);
}

void setupWindows(Hyperion *hyp)
{
    auto input = new ControlHubInput<MotorPosition>(
        8,
        &hyp->hub,
        {
            {.column = 1, .slot = 0, .pattern = new Window::StaticPattern("Open", 0, 1)},
            {.column = 1, .slot = 1, .pattern = new Window::StaticPattern("Closed top", 0, 0)},
            {.column = 1, .slot = 2, .pattern = new Window::StaticPattern("Closed mid", 0.5, 0.5)},
            {.column = 1, .slot = 3, .pattern = new Window::StaticPattern("Closed bottom", 1, 1)},
            {.column = 1, .slot = 4, .pattern = new Window::StaticPattern("Medium slit", 0.45, 0.55)},
            {.column = 1, .slot = 5, .pattern = new Window::StaticPattern("Small slit", 0.4, 0.6)},

            

            {.column = 2, .slot = 0, .pattern = new Window::GenericFallingPattern("Falling together", 0.5, 0, 64)},
            {.column = 2, .slot = 1, .pattern = new Window::GenericFallingPattern("Falling one by one", 0.5, 1, 32)},

            {.column = 2, .slot = 2, .pattern = new Window::GenericPositionLFOPattern<Cos>("Zig-zag together", 1, 0, 64)},
            {.column = 2, .slot = 3, .pattern = new Window::GenericPositionLFOPattern<Cos>("Zig-zag one by one", 1, 1, 64)},
            {.column = 2, .slot = 4, .pattern = new Window::GenericPositionLFOPattern<Cos>("Zig-zag pairs", 1, 4, 64)},

            {.column = 2, .slot = 5, .pattern = new Window::GenericGapLFOPattern<NegativeCosFast>("Gap snake", 1, 0.25, 32)},
            {.column = 2, .slot = 6, .pattern = new Window::GenericGapLFOPattern<NegativeCosFast>("Gap one by one", 1, 1, 32)},
            {.column = 2, .slot = 7, .pattern = new Window::GenericGapLFOPattern<NegativeCosFast>("Gap pair", 1, 4, 32)},

            // {.column = 2, .slot = 0, .pattern = new Window::SinPattern()},
            // {.column = 2, .slot = 1, .pattern = new Window::SinGapPattern()},
            // {.column = 2, .slot = 2, .pattern = new Window::FallingPattern()},
            // {.column = 2, .slot = 3, .pattern = new Window::SinTempoPattern()},
        });

    int nbytes = 8 * sizeof(MotorPosition);
    auto split = new InputSlicer(input, {
                                            {0, nbytes, true},
                                            {0, nbytes, true},
                                            {0, nbytes, true},
                                            {0, nbytes, true},
                                            {0, nbytes, false},
                                        });

    const char* hosts[4] = {
        "window1.local",  // "169.254.0.201",
        "window2.local",
        "window3.local",
        "window4.local",
    };
    for (int i=0;i<4;i++){
        hyp->addPipe(new ConvertPipe<MotorPosition, MotorPositionOut>(
            split->getInput(i),
            new ArtNetOutput(hosts[i], 0, 0, 1, 60)));
    }

    hyp->addPipe(new ConvertPipe<MotorPosition, MotorPositionOut>(
        split->getInput(split->size()-1),
        new WebsocketOutput(&hyp->webServer, "/ws/windows", 60)));

    hyp->hub.setForcedSelection(1, true);
    hyp->hub.setFlashColumn(1, false, true);
    hyp->hub.setFlashColumn(2, false, true);
    hyp->hub.setColumnName(1, "Static");
    hyp->hub.setColumnName(2, "Movement");
}

void setupLed(Hyperion *hyp)
{
    const int columnIndex1 = 3;
    const int columnIndex2 = 4;

    PixelMap *map = new PixelMap(
        resizeAndTranslateMap(gridMap(4, 1, 0.2, 0.2),1,0,-0.8)
    );

    auto input = new ControlHubInput<RGBA>(
        map->size(),
        &hyp->hub,
        {
            {.column = columnIndex1, .slot = 0, .pattern = new LedPatterns::OnPattern({255,255,255},"White")},
            {.column = columnIndex1, .slot = 1, .pattern = new LedPatterns::PalettePattern(0,"Primary")},
            {.column = columnIndex1, .slot = 2, .pattern = new LedPatterns::PalettePattern(1,"Secondary")},

            {.column = columnIndex2, .slot = 0, .pattern = new LedPatterns::SinPattern()},
            {.column = columnIndex2, .slot = 1, .pattern = new LedPatterns::GlowPulsePattern()},
            {.column = columnIndex2, .slot = 2, .pattern = new LedPatterns::GradientChasePattern()},
            {.column = columnIndex2, .slot = 3, .pattern = new LedPatterns::FlashesPattern()},
            {.column = columnIndex2, .slot = 4, .pattern = new LedPatterns::PixelGlitchPattern()},
            {.column = columnIndex2, .slot = 5, .pattern = new LedPatterns::StrobePattern()},
            {.column = columnIndex2, .slot = 6, .pattern = new LedPatterns::StrobeHighlightPattern()},
        }
    );
    distributeAndMonitor<RGBW>(hyp, input, map, {
        {"hyperslave5.local", 9619, (int) map->size()},
    }, nullptr, 0.05);

    hyp->hub.setColumnName(columnIndex1, "Led bg");
    hyp->hub.setColumnName(columnIndex2, "Led fg");
}

void setupBulbs(Hyperion *hyp)
{
    const int columnIndex1 = 5;
    const int columnIndex2 = 6;

    PixelMap *map = new PixelMap(
        resizeAndTranslateMap(gridMap(4, 6, 0.2, 0.2),1,0,0)
    );

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        {
            {.column = columnIndex1, .slot = 0, .pattern = new MonochromePatterns::OnPattern(50)},
            {.column = columnIndex1, .slot = 1, .pattern = new MonochromePatterns::LFOPattern<NegativeCosFast>("Sin")},
            {.column = columnIndex1, .slot = 2, .pattern = new MonochromePatterns::LFOPattern<SawDown>("SawDown")},
            {.column = columnIndex1, .slot = 3, .pattern = new MonochromePatterns::GlowPattern()},
            {.column = columnIndex1, .slot = 4, .pattern = new MonochromePatterns::BeatStepPattern()},
            {.column = columnIndex1, .slot = 5, .pattern = new MonochromePatterns::BeatSingleFadePattern()},
            {.column = columnIndex1, .slot = 6, .pattern = new MonochromePatterns::BeatMultiFadePattern()},
            {.column = columnIndex1, .slot = 7, .pattern = new MonochromePatterns::BeatShakePattern()},

            
            {.column = columnIndex2, .slot = 0, .pattern = new MonochromePatterns::BlinderPattern()},
            {.column = columnIndex2, .slot = 1, .pattern = new MonochromePatterns::BeatAllFadePattern()},
            {.column = columnIndex2, .slot = 2, .pattern = new MonochromePatterns::GlitchPattern()},
            {.column = columnIndex2, .slot = 3, .pattern = new MonochromePatterns::SlowStrobePattern()},
        }
    );

    // hyp->addPipe(new ConvertPipe<Monochrome, Monochrome>(
    //     input,
    //     new UDPOutput("hyperslave5.local", 9619, 60)));

    distributeAndMonitor<Monochrome>(hyp, input, map, {
        {"hyperslave6.local", 9619, (int) map->size()},
    }, nullptr, 0.05);

    hyp->hub.setColumnName(columnIndex1, "Bulbs");
    hyp->hub.setColumnName(columnIndex2, "Bulbs flash");
    hyp->hub.setFlashColumn(columnIndex2);
}

int main()
{
    auto hyp = new Hyperion();

    //add constant temposource
    Tempo::AddSource(new ConstantTempo(120));

    setupPalette(hyp);
    setupWindows(hyp);
    setupLed(hyp);
    setupBulbs(hyp);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
