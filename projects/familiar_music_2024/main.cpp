#include "core/hyperion.hpp"
#include "mapping/chandelierMap3d.hpp"
#include "patterns/common.hpp"
#include "mapping/zigzagIndices.hpp"
#include "patterns/chandelier.hpp"
#include "patterns/mask.hpp"
#include "patterns/debug.hpp"
#include "setViewParams.hpp"

//2.7 gamma experimentally determined in dark environment
LUT *ledbarLut = new ColourCorrectionLUT(2.7, 255, 255, 255, 255);

auto cchandelierMap3d = chandelierMap3d.toCylindricalXZ();

void addPaletteColumn(Hyperion *hyp);
void addChandelierPipe(Hyperion *);

#define COL_PALETTE 0
#define COL_CHANDELIER_1 1
#define COL_CHANDELIER_2 2
#define COL_CHANDELIER_3 3
#define COL_CHANDELIER_MASK 4
#define COL_CHANDELIER_FLASH 7
#define COL_DEBUG 8

int main()
{
    auto hyp = new Hyperion();
    
    addPaletteColumn(hyp);
    addChandelierPipe(hyp);

    // Tempo::AddSource(new ConstantTempo(120));

    // select first palette
    hyp->hub.buttonPressed(COL_PALETTE, 0);
    hyp->hub.setFlashColumn(COL_PALETTE, false, true);
    hyp->hub.setForcedSelection(COL_PALETTE);

    hyp->hub.setFlashColumn(COL_CHANDELIER_FLASH, true, false);

    hyp->hub.setColumnName(COL_PALETTE, "Palette");
    hyp->hub.setColumnName(COL_CHANDELIER_1, "Bottom");
    hyp->hub.setColumnName(COL_CHANDELIER_2, "Mid");
    hyp->hub.setColumnName(COL_CHANDELIER_3, "Top");
    hyp->hub.setColumnName(COL_CHANDELIER_MASK, "Mask");
    hyp->hub.setColumnName(COL_CHANDELIER_FLASH, "Flash");
    hyp->hub.setColumnName(COL_DEBUG, "Debug");

    hyp->start();
    setViewParams(hyp);

    while (1)
        Thread::sleep(60 * 1000);
}

void addChandelierPipe(Hyperion *hyp)
{
    int nleds = chandelierMap3d.size();
    int nbytes = nleds * sizeof(RGBA);

    auto input = new ControlHubInput<RGBA>(
        nleds,
        &hyp->hub,
        {
            {.column = COL_CHANDELIER_1, .slot = 0, .pattern = new Patterns::StaticGradientPattern(&cchandelierMap3d)},
            {.column = COL_CHANDELIER_1, .slot = 1, .pattern = new Patterns::BreathingGradientPattern(&cchandelierMap3d)},
            {.column = COL_CHANDELIER_1, .slot = 2, .pattern = new Patterns::VerticallyIsolated(&chandelierMap3d)},
            {.column = COL_CHANDELIER_1, .slot = 3, .pattern = new Patterns::GrowShrink(&chandelierMap3d)},
            {.column = COL_CHANDELIER_1, .slot = 4, .pattern = new Patterns::Lighthouse(&cchandelierMap3d)},
            {.column = COL_CHANDELIER_1, .slot = 5, .pattern = new Patterns::BarLFO()},
            {.column = COL_CHANDELIER_1, .slot = 6, .pattern = new Patterns::RibbenClivePattern<NegativeCosFast>()},
            {.column = COL_CHANDELIER_1, .slot = 7, .pattern = new Patterns::RibbenFlashPattern()},

            {.column = COL_CHANDELIER_2, .slot = 0, .pattern = new Patterns::GradientLFO()},
            {.column = COL_CHANDELIER_2, .slot = 1, .pattern = new Patterns::FadeFromRandomChandelier()},
            {.column = COL_CHANDELIER_2, .slot = 2, .pattern = new Patterns::RotatingRingsPattern(&chandelierMap3d)},
            {.column = COL_CHANDELIER_2, .slot = 3, .pattern = new Patterns::OnBeatWindowChaseUpPattern(&chandelierMap3d)},
            {.column = COL_CHANDELIER_2, .slot = 4, .pattern = new Patterns::HorizontalSin(&chandelierMap3d)},
            {.column = COL_CHANDELIER_2, .slot = 5, .pattern = new Patterns::VerticalSin(&chandelierMap3d)},
            {.column = COL_CHANDELIER_2, .slot = 6, .pattern = new Patterns::SinChase2Pattern()},
            {.column = COL_CHANDELIER_2, .slot = 7, .pattern = new Patterns::FadeFromCenter()},
            // {.column = COL_CHANDELIER_2, .slot = 9, .pattern = new Patterns::SawChasePattern()},
            // {.column = COL_CHANDELIER_2, .slot = 10, .pattern = new Patterns::CeilingChase()},
            
            {.column = COL_CHANDELIER_3, .slot = 0, .pattern = new Patterns::RadialGlitterFadePattern(&cchandelierMap3d)},
            {.column = COL_CHANDELIER_3, .slot = 1, .pattern = new Patterns::SegmentChasePattern()},
            {.column = COL_CHANDELIER_3, .slot = 2, .pattern = new Patterns::GlowPulsePattern()},
            {.column = COL_CHANDELIER_3, .slot = 3, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_CHANDELIER_3, .slot = 4, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_CHANDELIER_3, .slot = 5, .pattern = new Patterns::LineLaunch(&chandelierMap3d)},
            {.column = COL_CHANDELIER_3, .slot = 6, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_CHANDELIER_3, .slot = 7, .pattern = new Patterns::FlashesPattern()},
            // {.column = COL_CHANDELIER_FG, .slot = 8, .pattern = new Patterns::StrobePattern()},
            // {.column = COL_CHANDELIER_FG, .slot = 8, .pattern = new Patterns::StrobeHighlightPattern()},

            {.column = COL_CHANDELIER_MASK, .slot = 0, .pattern = new Patterns::GlowPulseMaskPattern()},
            {.column = COL_CHANDELIER_MASK, .slot = 1, .pattern = new Patterns::SideWaveMask(nullptr)},
            {.column = COL_CHANDELIER_MASK, .slot = 2, .pattern = new Patterns::SideWaveMask(zigzagIndices)}, 
            {.column = COL_CHANDELIER_MASK, .slot = 3, .pattern = new Patterns::RotatingRingsMaskPattern(&chandelierMap3d)},
            {.column = COL_CHANDELIER_MASK, .slot = 4, .pattern = new Patterns::SinChaseMaskPattern(zigzagIndices)},
            {.column = COL_CHANDELIER_MASK, .slot = 5, .pattern = new Patterns::SinChaseMaskPattern(nullptr)},
            // {.column = COL_CHANDELIER_MASK, .slot = 6, .pattern = new Patterns::GlowPulseMaskPattern()},

            {.column = COL_CHANDELIER_FLASH, .slot = 0, .pattern = new Patterns::GlitchPattern()},
            {.column = COL_CHANDELIER_FLASH, .slot = 1, .pattern = new Patterns::PixelGlitchPattern()},
            {.column = COL_CHANDELIER_FLASH, .slot = 2, .pattern = new Patterns::LineLaunch(&chandelierMap3d)},
            {.column = COL_CHANDELIER_FLASH, .slot = 3, .pattern = new Patterns::FadingNoisePattern()},
            {.column = COL_CHANDELIER_FLASH, .slot = 4, .pattern = new Patterns::SegmentGlitchPattern()},
            {.column = COL_CHANDELIER_FLASH, .slot = 5, .pattern = new Patterns::FlashesPattern()},
            {.column = COL_CHANDELIER_FLASH, .slot = 6, .pattern = new Patterns::StrobePattern()},
            {.column = COL_CHANDELIER_FLASH, .slot = 7, .pattern = new Patterns::StrobeHighlightPattern()},
            // {.column = COL_CHANDELIER_FLASH, .slot = 8, .pattern = new Patterns::RadialGlitterFadePattern(&cchandelierMap3d)},

            {.column = COL_DEBUG, .slot = 0, .pattern = new Patterns::ShowStarts(nleds/60/8)},
            {.column = COL_DEBUG, .slot = 1, .pattern = new Patterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = COL_DEBUG, .slot = 2, .pattern = new Patterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = COL_DEBUG, .slot = 3, .pattern = new Patterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = COL_DEBUG, .slot = 4, .pattern = new Patterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = COL_DEBUG, .slot = 5, .pattern = new Patterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = COL_DEBUG, .slot = 6, .pattern = new Patterns::Gamma(60)},
        });

    auto splitInput = new InputSlicer(
        input,
        {
            {0 * nbytes / 8, nbytes / 8, true},
            {1 * nbytes / 8, nbytes / 8, true},
            {2 * nbytes / 8, nbytes / 8, true},
            {3 * nbytes / 8, nbytes / 8, true},
            {4 * nbytes / 8, nbytes / 8, true},
            {5 * nbytes / 8, nbytes / 8, true},
            {6 * nbytes / 8, nbytes / 8, true},
            {7 * nbytes / 8, nbytes / 8, true},
            {0, nbytes, false},
        });

    for (int i = 0; i < splitInput->size() - 1; i++)
    {
        auto pipe = new ConvertPipe<RGBA, BGR>(
            splitInput->getInput(i),
            new UDPOutput("hyperslave4.local", 9611 + i, 60),
            ledbarLut);
        hyp->addPipe(pipe);
    }

    hyp->addPipe(
        new ConvertPipe<RGBA, RGB>(
            splitInput->getInput(splitInput->size() - 1),
            new MonitorOutput3d(&hyp->webServer, &chandelierMap3d, 60, 0.03)));
}

void addPaletteColumn(Hyperion *hyp)
{
    auto paletteColumn = new PaletteColumn(
        &hyp->hub,
        COL_PALETTE,
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
            &plumBath,
            &matrix,

            &sunset6,
            &sunset7,
            &sunset1,
            &coralTeal,
            &deepBlueOcean,
            &redSalvation,
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
