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

            {.column = 2, .slot = 0, .pattern = new Window::SinPattern()},
            {.column = 2, .slot = 1, .pattern = new Window::SinGapPattern()},
            {.column = 2, .slot = 2, .pattern = new Window::FallingPattern()},
        });

    int nbytes = 8 * sizeof(MotorPosition);
    auto split = new InputSlicer(input, {
                                            {0, nbytes, true},
                                            {0, nbytes, false},
                                        });

    hyp->addPipe(new ConvertPipe<MotorPosition, MotorPositionOut>(
        split->getInput(0),
        new ArtNetOutput("169.254.0.201", 0, 0, 1, 60)));

    hyp->addPipe(new ConvertPipe<MotorPosition, MotorPositionOut>(
        split->getInput(1),
        new WebsocketOutput(&hyp->webServer, "/ws/windows", 60)));

    hyp->hub.setForcedSelection(1, true);
    hyp->hub.setFlashColumn(1, false, true);
    hyp->hub.setFlashColumn(2, false, true);
    hyp->hub.setColumnName(1, "Static");
    hyp->hub.setColumnName(2, "Movement");
}

void setupLed(Hyperion *hyp)
{
    const int columnIndex = 3;

    PixelMap *map = new PixelMap(
        resizeAndTranslateMap(gridMap(4, 1, 0.2),1,0,-0.8)
    );

    auto input = new ControlHubInput<RGBA>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            new LedPatterns::OnPattern({255,255,255},"White"),
            new LedPatterns::PalettePattern(0,"Primary"),
            new LedPatterns::PalettePattern(1,"Secondary"),
            new LedPatterns::GlowPulsePattern(),
            new LedPatterns::SegmentChasePattern(),
            new LedPatterns::FlashesPattern(),
            new LedPatterns::StrobePattern(),
            new LedPatterns::PixelGlitchPattern(),
            new LedPatterns::FadingNoisePattern(),
            new LedPatterns::StrobeHighlightPattern(),
        }
    );
    distributeAndMonitor<RGBW>(hyp, input, map, {
        {"hyperslave3.local", 9619, (int) map->size()},
    }, nullptr, 0.05);

    hyp->hub.setColumnName(columnIndex, "Led");
}

void setupBulbs(Hyperion *hyp)
{
    const int columnIndex = 4;

    PixelMap *map = new PixelMap(
        resizeAndTranslateMap(gridMap(4, 6, 0.2),1,0,0)
    );

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        columnIndex,
        {
            new MonochromePatterns::SinPattern(),
            new MonochromePatterns::GlowPattern(),
            new MonochromePatterns::FastStrobePattern(),
            new MonochromePatterns::SlowStrobePattern(),
            new MonochromePatterns::FastStrobePattern2(),
            new MonochromePatterns::BlinderPattern(),
            new MonochromePatterns::BeatAllFadePattern(),
            new MonochromePatterns::BeatShakePattern(),
            new MonochromePatterns::BeatSingleFadePattern(),
            new MonochromePatterns::BeatMultiFadePattern(),
            new MonochromePatterns::GlitchPattern(),
            new MonochromePatterns::OnPattern(255),
            new MonochromePatterns::LFOPattern<NegativeCosFast>("Sin"),
            new MonochromePatterns::LFOPattern<SawDown>("SawDown"),
            new MonochromePatterns::BeatStepPattern(),
            new MonochromePatterns::GlowOriginalPattern(),
        }
    );
    distributeAndMonitor<Monochrome>(hyp, input, map, {
        {"hyperslave4.local", 9619, (int) map->size()},
    }, nullptr, 0.05);

    hyp->hub.setColumnName(columnIndex, "Bulbs");
}

int main()
{
    auto hyp = new Hyperion();

    setupPalette(hyp);
    setupWindows(hyp);
    setupLed(hyp);
    setupBulbs(hyp);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
