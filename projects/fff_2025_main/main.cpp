#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/paletteColumn.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-mapped-2d.hpp"
#include "../common/patterns/patterns-mask.hpp"
#include "../common/patterns/patterns-monochrome.hpp"
#include "../common/patterns/patterns-test.hpp"
#include "../common/setViewParams.hpp"
#include "core/hyperion.hpp"
#include "mapping/cageMap.hpp"
#include "mapping/wingMap.old.hpp"

#include "buttonMidiControllerFactory.hpp"

// TODO write own pattern for this project
#include "patterns-button.hpp"
#include "patterns-flash.hpp"
#include "patterns-hi.hpp"
#include "patterns-low.hpp"
#include "patterns-max.hpp"
#include "patterns-mid.hpp"
#include "patterns-min.hpp"
#include "patterns-wings.hpp"

LUT *ledBarLut = new ColourCorrectionLUT(1.8, 255, 200, 200, 200);

PixelMap3d::Cylindrical cCageMap = cageMap.toCylindricalXZ();
PixelMap3d::Spherical sCageMap = cageMap.toSphericalXZ();
PixelMap::Polar pWingMap = wingMap.toPolarRotate90();

enum Columns {
    PALETTE,
    CAGE_1,
    CAGE_2,
    CAGE_3,
    CAGE_4,
    CAGE_5,
    CAGE_MASK,
    CAGE_FLASH,

    WINGS_1,
    WINGS_2,
    WINGS_3,
    WINGS_MASK,
    WINGS_FLASH,
    EFFECTS,
    BUTTONS,
    BUTTONS_EFFECT,
};

void addCagePipe(Hyperion *hyp)
{

    PixelMap3d *map = &cageMap;
    PixelMap3d::Cylindrical *cmap = &cCageMap;
    PixelMap3d::Spherical *smap = &sCageMap;

    std::vector<Slave> distribution = {
        {"hypernode1.local", 9611, 8 * 60},
        {"hypernode1.local", 9612, 8 * 60},
        {"hypernode1.local", 9613, 8 * 60},
        {"hypernode1.local", 9614, 8 * 60},
        {"hypernode1.local", 9615, 8 * 60},
        {"hypernode1.local", 9616, 8 * 60},

        {"hypernode2.local", 9611, 3 * 60},
        {"hypernode2.local", 9612, 3 * 60},
        {"hypernode2.local", 9613, 3 * 60},
        {"hypernode2.local", 9614, 3 * 60},
        {"hypernode2.local", 9615, 3 * 60},
        {"hypernode2.local", 9616, 3 * 60},

    };

    auto input = new ControlHubInput<RGBA>(

        map->size(),
        &hyp->hub,
        {
            {.column = 1, .slot = 0, .pattern = new Max::ChevronsPattern(map)},
            {.column = 1, .slot = 1, .pattern = new Max::ChevronsConePattern(cmap)},
            {.column = 1, .slot = 2, .pattern = new Max::RadialFadePattern(cmap)},
            {.column = 1, .slot = 3, .pattern = new Max::RadialGlitterFadePattern(cmap)},
            {.column = 1, .slot = 4, .pattern = new Max::AngularFadePattern(cmap)},
            {.column = 1, .slot = 5, .pattern = new Max::GrowingStrobePattern(cmap)},

            {.column = 2, .slot = 0, .pattern = new Min::RibbenClivePattern<Glow>(10000, 1, 0.15)},
            {.column = 2, .slot = 1, .pattern = new Min::RibbenFlashPattern()},
            {.column = 2, .slot = 2, .pattern = new Min::GrowingCirclesPattern(map)},
            {.column = 2, .slot = 3, .pattern = new Min::SpiralPattern(cmap)},
            {.column = 2, .slot = 4, .pattern = new Min::SegmentChasePattern()},
            {.column = 2, .slot = 5, .pattern = new Min::GlowPulsePattern()},
            {.column = 2, .slot = 6, .pattern = new Min::LineLaunch(map)},

            {.column = 3, .slot = 0, .pattern = new Low::HorizontalSin(cmap)},
            {.column = 3, .slot = 1, .pattern = new Low::VerticallyIsolated(cmap)},
            {.column = 3, .slot = 2, .pattern = new Low::HorizontalSaw(cmap)},
            {.column = 3, .slot = 3, .pattern = new Low::StaticGradientPattern(map)},
            {.column = 3, .slot = 4, .pattern = new Low::OnBeatColumnChaseUpPattern(map)},
            {.column = 3, .slot = 5, .pattern = new Low::GrowShrink(cmap)},
            {.column = 3, .slot = 6, .pattern = new Low::RotatingRingsPattern(cmap)},
            {.column = 3, .slot = 7, .pattern = new Low::GlowPulsePattern(map)},

            // {.column = 4, .slot = 0, .pattern = new Mid::HaloOnBeat(cmap)},
            {.column = 4, .slot = 1, .pattern = new Mid::Halo(cmap)},
            {.column = 4, .slot = 2, .pattern = new Mid::SnowflakePatternColumn(cmap)},
            {.column = 4, .slot = 3, .pattern = new Mid::TakkenChase(cmap)},
            {.column = 4, .slot = 5, .pattern = new Mid::Lighthouse(cmap)},
            {.column = 4, .slot = 6, .pattern = new Mid::FireworksPattern(map)},
            {.column = 4, .slot = 7, .pattern = new Mid::DoubleFlash(cmap)},

            {.column = 5, .slot = 0, .pattern = new Hi::DotBeatPattern(cmap)},
            {.column = 5, .slot = 5, .pattern = new Hi::XY(map)},

            {.column = 7, .slot = 0, .pattern = new Flash::FlashesPattern()},
            {.column = 7, .slot = 1, .pattern = new Flash::SquareGlitchPattern(map)},
            {.column = 7, .slot = 3, .pattern = new Flash::PixelGlitchPattern()},
            {.column = 7, .slot = 4, .pattern = new Max::GrowingStrobePattern(cmap)},
            {.column = 7, .slot = 5, .pattern = new Flash::StrobeHighlightPattern()},
            {.column = 7, .slot = 6, .pattern = new Flash::StrobePattern()},
            {.column = 7, .slot = 7, .pattern = new Flash::FadingNoisePattern()},

            // {.column = 8, .slot = 0, .pattern = new TestPatterns::ShowStarts(60)},
            // {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            // {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            // {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            // {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            // {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            // {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(120, 20)},
            // {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(60)},
            // {.column = 8, .slot = 8, .pattern = new TestPatterns::BrightnessMatch()},

            // // Static
            // {.column = 1, .slot = 0, .pattern = new LedPatterns::PalettePattern(0, "Primary")},
            // {.column = 1, .slot = 1, .pattern = new LedPatterns::PalettePattern(1, "Secondary")},

            {.column = 8, .slot = 0, .pattern = new TestPatterns::DistributionPattern(distribution)},
            {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
            {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(10)},

            {.column = 9, .slot = 0, .pattern = new Buttons::ButtonPressedPattern(map, 0)},
            {.column = 9, .slot = 1, .pattern = new Buttons::ButtonPressedPattern(map, 1)},
            {.column = 9, .slot = 2, .pattern = new Buttons::ButtonPressedPattern(map, 2)},
            {.column = 9, .slot = 3, .pattern = new Buttons::ButtonPressedPattern(map, 3)},
            {.column = 9, .slot = 4, .pattern = new Buttons::ButtonPressedPattern(map, 4)},
            {.column = 9, .slot = 5, .pattern = new Buttons::ButtonPressedPattern(map, 5)},
            {.column = 9, .slot = 6, .pattern = new Buttons::SyncToMeasurePattern(smap)},
            {.column = 9, .slot = 7, .pattern = new Buttons::FadingNoisePattern()},
            {.column = 9, .slot = 8, .pattern = new Buttons::StrobeFadePattern(smap)},

        });

    distributeAndMonitor3d<BGR, RGBA>(
        hyp, input, map, distribution, ledBarLut);

    hyp->hub.setColumnName(1, "Static");

    //   hyp->hub.buttonPressed(8,0);

    // hyp->hub.buttonPressed(9,0);
    // hyp->hub.buttonPressed(9,1);
    // hyp->hub.buttonPressed(9,2);
    // hyp->hub.buttonPressed(9,3);
    // hyp->hub.buttonPressed(9,4);

    // hyp->hub.buttonPressed(9,6);
    // // hyp->hub.buttonPressed(9,7);
    // hyp->hub.buttonPressed(9,8);
}

void addWingsPipe(Hyperion *hyp)
{
    PixelMap *map = &wingMap;
    PixelMap::Polar *pmap = &pWingMap;
    PixelMap *displayMap = new PixelMap(resizeAndTranslateMap(wingMap, 0.9));
    IndexMap *zigzag = new ZigZagMapper(60, true);

    std::vector<Slave> distribution = {
        {"hypernode2.local", 9611, 8 * 60},
        {"hypernode2.local", 9612, 8 * 60},
        {"hypernode2.local", 9613, 8 * 60},
        {"hypernode2.local", 9614, 8 * 60},
    };

    auto input = new ControlHubInput<RGBA>(

        map->size(),
        &hyp->hub,
        {

            {
                .column = 10,
                .slot = 0,
                .pattern = new LedPatterns::OnPattern({255, 255, 255}, "White"),
            },
            {
                .column = 10,
                .slot = 1,
                .pattern = new LedPatterns::PalettePattern(0, "Primary"),
            },
            {
                .column = 10,
                .slot = 2,
                .pattern = new LedPatterns::PalettePattern(1, "Secondary"),
            },
            {
                .column = 10,
                .slot = 3,
                .pattern = new LedPatterns::GlowPulsePattern(),
            },
            {
                .column = 10,
                .slot = 4,
                .pattern = new LedPatterns::SegmentChasePattern(),
            },
            {
                .column = 10,
                .slot = 5,
                .pattern = new LedPatterns::FlashesPattern(),
            },
            {
                .column = 10,
                .slot = 6,
                .pattern = new LedPatterns::StrobePattern(),
            },
            {
                .column = 10,
                .slot = 7,
                .pattern = new LedPatterns::PixelGlitchPattern(),
            },
            {
                .column = 10,
                .slot = 8,
                .pattern = new LedPatterns::FadingNoisePattern(),
            },
            {
                .column = 10,
                .slot = 9,
                .pattern = new LedPatterns::StrobeHighlightPattern(),
            },
            {
                .column = 10,
                .slot = 10,
                .pattern = new LedPatterns::GradientChasePattern(),
                .indexMap = zigzag
            },
            {.column = 10, .slot = 11, .pattern = new LedPatterns::RibbenClivePattern<Glow>(10000, 1, 0.15)},
            {.column = 10, .slot = 12, .pattern = new LedPatterns::ClivePattern<SawDown>(32, 1000, 1, 0.1)},
            {.column = 10, .slot = 13, .pattern = new LedPatterns::ClivePattern<SoftPWM>(32, 1000, 1, 0.5)},
            {.column = 10, .slot = 14, .pattern = new LedPatterns::ClivePattern<Glow>(32, 1000, 1, 0.5)},
            {.column = 10, .slot = 15, .pattern = new LedPatterns::BarChase(60), .indexMap = zigzag},
            {.column = 10, .slot = 16, .pattern = new Wings::FadeFromCenter()},
            {.column = 10, .slot = 17, .pattern = new LedPatterns::FadeFromRandom()},
            {.column = 10, .slot = 18, .pattern = new LedPatterns::SideWave(60), .indexMap = zigzag},
            {.column = 10, .slot = 19, .pattern = new LedPatterns::SinChasePattern(60), .indexMap = zigzag},
            {.column = 10, .slot = 20, .pattern = new LedPatterns::Fireworks()},
            {.column = 10, .slot = 21, .pattern = new LedPatterns::SegmentGlitchPattern()},

            {.column = 11, .slot = 0, .pattern = new Mapped2dPatterns::Lighthouse(pmap)},
            {
                .column = 11,
                .slot = 1,
                .pattern = new Mapped2dPatterns::GrowingCirclesPattern(map),
            },
            {
                .column = 11,
                .slot = 2,
                .pattern = new Mapped2dPatterns::LineLaunch(map),
            },
            {
                .column = 11,
                .slot = 3,
                .pattern = new Mapped2dPatterns::SpiralPattern(pmap),
            },
            {
                .column = 11,
                .slot = 4,
                .pattern = new Mapped2dPatterns::DotBeatPattern(pmap),
            },
            {
                .column = 11,
                .slot = 5,
                .pattern = new Mapped2dPatterns::HorizontalSin(pmap),
            },
            {
                .column = 11,
                .slot = 6,
                .pattern = new Mapped2dPatterns::HorizontalSaw(pmap),
            },
            {
                .column = 11,
                .slot = 7,
                .pattern = new Mapped2dPatterns::RadialSaw(pmap),
            },
            //   {.column = 11, .slot = 8, .pattern = new Mapped2dPatterns::GrowShrink(pmap),},
            {.column = 11, .slot = 10, .pattern = new Mapped2dPatterns::HorizontalGradientPattern(map)},
            {.column = 11, .slot = 11, .pattern = new Mapped2dPatterns::RadialGlitterFadePattern(pmap)},
            {.column = 11, .slot = 12, .pattern = new Mapped2dPatterns::RadialFadePattern(pmap)},
            {.column = 11, .slot = 13, .pattern = new Mapped2dPatterns::AngularFadePattern(pmap)},
            {.column = 11, .slot = 14, .pattern = new Wings::XY(map)},

            {.column = 12, .slot = 0, .pattern = new MaskPatterns::SinChaseMaskPattern()},
            {.column = 12, .slot = 1, .pattern = new MaskPatterns::SinChaseMaskPattern(), .indexMap = zigzag},
            {.column = 12, .slot = 2, .pattern = new MaskPatterns::GlowPulseMaskPattern()},
            {.column = 12, .slot = 3, .pattern = new MaskPatterns::SegmentGradientMaskPattern(60, true), .indexMap = zigzag},

            {.column = 8, .slot = 0, .pattern = new TestPatterns::DistributionPattern(distribution, 60)},
            {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
            {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(10)},

        });

    distributeAndMonitor<BGR, RGBA>(
        hyp, input, displayMap, distribution, ledBarLut);

    hyp->hub.setColumnName(8, "Wings");
}

void addVulturePipe(Hyperion *hyp)
{

    const int wingSize = 50;

    PixelMap *map = new PixelMap(combineMaps({
        gridMap(wingSize, 3, -0.01, 0.07, -0.5, -0.5),
        gridMap(wingSize, 3, 0.01, 0.07, 0.5, -0.5),
        circleMap(100, 0.25, 0, -0.5),
    }));

    Distribution distribution = {
        {"hypernode3.local", 9611, wingSize},
        {"hypernode3.local", 9612, wingSize},
        {"hypernode3.local", 9613, wingSize},
        {"hypernode3.local", 9614, wingSize},
        {"hypernode3.local", 9615, wingSize},
        {"hypernode3.local", 9616, wingSize},
        {"hypernode3.local", 9617, 100},
    };

    auto input = new ControlHubInput<RGBA>(

        map->size(),
        &hyp->hub,
        {
            {.column = 8, .slot = 0, .pattern = new TestPatterns::DistributionPattern(distribution, 100)},
            {.column = 8, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = 8, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = 8, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = 8, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = 8, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = 8, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
            {.column = 8, .slot = 7, .pattern = new TestPatterns::Gamma(10)},

        });

    distributeAndMonitor<BGR, RGBA>(
        hyp, input, map, distribution, ledBarLut);

    hyp->hub.setColumnName(8, "Wings");
}

void addDMXPipe(Hyperion *hyp)
{
    PixelMap *map = new PixelMap(combineMaps({PixelMap({{.x = -0.02, .y = 0}, {.x = 0.02, .y = 0}}), // eyes
                                              gridMap(4, 1, 0.1, 0.1, 0, 0.85)}));                   // pinspots

    Distribution distribution = {{"hypernode3.local", 9611, (int)map->size()}};

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        {
            {.column = 8, .slot = 4, .pattern = new MonochromePatterns::OnPattern()},
        });

    distributeAndMonitor<Monochrome, Monochrome>(
        hyp, input, map, distribution);
}

void addFogPipe(Hyperion *hyp)
{
    PixelMap *map = new PixelMap({{.x = -0, .y = 0.5}});

    Distribution distribution = {{"hypernode4.local", 9611, (int)map->size()}};

    auto input = new ControlHubInput<Effect>(
        map->size(),
        &hyp->hub,
        {
            //{.column = 8, .slot = 4, .pattern = new MonochromePatterns::OnPattern()},
        });

    distributeAndMonitor<Effect, Effect>(
        hyp, input, map, distribution, nullptr, 0.03);
}

void addLightningPipe(Hyperion *hyp)
{
    PixelMap *map = new PixelMap(gridMap(8, 1, 0.1, 0.1, 0, 0.7));

    Distribution distribution = {{"hypernode4.local", 9621, (int)map->size()}};

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        {
            {.column = 8, .slot = 4, .pattern = new MonochromePatterns::OnPattern()},
        });

    distributeAndMonitor<Monochrome, Monochrome>(
        hyp, input, map, distribution);
}

int main()
{
    auto hyp = new Hyperion();
    hyp->setMidiControllerFactory(new ButtonMidiControllerFactory());

    Tempo::AddSource(new ConstantTempo(120));

    setupPaletteColumn(hyp);
    addCagePipe(hyp);
    addWingsPipe(hyp);
    addVulturePipe(hyp);
    addFogPipe(hyp);
    addDMXPipe(hyp);
    addLightningPipe(hyp);

    hyp->start();
    // setViewParams(hyp, viewParamsDefault);
    // setViewParams(hyp, viewParamsTop);
    // setViewParams(hyp, viewParamsFront);
    // setViewParams(hyp, viewParamsSide);
    auto viewParams = new ViewParams(
        35,
        -0.45,
        Vector{0, 0.1, -2.5},
        Rotation{.5, 1, 0, 0},
        Rotation{0, 0, 1, 0});
    setViewParams(hyp, viewParams);

    //   auto viewParamsBottom = new ViewParams(
    //       100,
    //       -0.45,
    //       Vector{0, 0.0, -0.5},
    //       Rotation{-M_PI_2, 1, 0, 0},
    //       Rotation{0, 0, 1, 0});
    //   setViewParams(hyp, viewParamsBottom);

    while (1)
    {
        Thread::sleep(1000);
    }
}
