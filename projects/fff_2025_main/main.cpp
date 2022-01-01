#include "../common/distributeAndMonitor.hpp"
#include "../common/mapHelpers.hpp"
#include "../common/paletteColumn.hpp"
#include "../common/patterns/patterns-led.hpp"
#include "../common/patterns/patterns-mapped-2d.hpp"
#include "../common/patterns/patterns-mask.hpp"
#include "../common/patterns/patterns-monochrome.hpp"
#include "../common/patterns/patterns-test.hpp"
#include "../common/patterns/patterns-effect.hpp"
#include "../common/setViewParams.hpp"
#include "core/hyperion.hpp"
#include "mapping/cageMap.hpp"
#include "mapping/wingMap.old.hpp"
#include "columns.hpp"
#include "buttonMidiControllerFactory.hpp"

#include "patterns-button.hpp"
#include "patterns-flash.hpp"
#include "patterns-hi.hpp"
#include "patterns-low.hpp"
#include "patterns-max.hpp"
#include "patterns-mid.hpp"
#include "patterns-min.hpp"
#include "patterns-wings.hpp"
#include "patterns-mask.hpp"

LUT *ledBarLut = new ColourCorrectionLUT(1.8, 255, 200, 200, 200);

PixelMap3d::Cylindrical cCageMap = cageMap.toCylindricalXZ();
PixelMap3d::Cylindrical cCageMap90 = cCageMap.rotate(M_PI * 35. /180.);
PixelMap3d::Spherical sCageMap = cageMap.toSphericalXZ();
PixelMap::Polar pWingMap = wingMap.toPolarRotate90();


void addCagePipe(Hyperion *hyp)
{

    PixelMap3d *map = &cageMap;
    PixelMap3d::Cylindrical *cmap = &cCageMap;
    PixelMap3d::Cylindrical *cmap90 = &cCageMap90;
    PixelMap3d::Spherical *smap = &sCageMap;

    std::vector<Slave> distribution = {
        {"hyperslave1.local", 9611, 8 * 60},
        {"hyperslave1.local", 9612, 8 * 60},
        {"hyperslave1.local", 9613, 8 * 60},
        {"hyperslave1.local", 9614, 8 * 60},
        {"hyperslave1.local", 9615, 8 * 60},
        {"hyperslave1.local", 9616, 5 * 60},
        {"hyperslave2.local", 9617, 5 * 60},
        {"hyperslave2.local", 9618, 5 * 60},

        {"hyperslave2.local", 9611, 5 * 60},
        {"hyperslave2.local", 9612, 5 * 60},
        {"hyperslave2.local", 9613, 5 * 60},
        {"hyperslave2.local", 9614, 5 * 60},

    };

    std::vector<ControlHubInput<RGBA>::SlotPattern> patterns = {
        {.column = Columns::CAGE_1, .slot = 0, .pattern = new Max::ChevronsConePattern(cmap)},
        {.column = Columns::CAGE_1, .slot = 1, .pattern = new Low::VerticallyIsolated(cmap)}, 
        {.column = Columns::CAGE_1, .slot = 2, .pattern = new Low::HorizontalSaw(cmap)},
        {.column = Columns::CAGE_1, .slot = 3, .pattern = new Low::StaticGradientPattern(map)},
        {.column = Columns::CAGE_1, .slot = 4, .pattern = new Min::RibbenClivePattern<Glow>(10000, 1, 0.15)},
        {.column = Columns::CAGE_1, .slot = 5, .pattern = new Min::RibbenFlashPattern()},
        {.column = Columns::CAGE_1, .slot = 6, .pattern = new Low::HorizontalSin(cmap90)}, 
        {.column = Columns::CAGE_1, .slot = 7, .pattern = new Mid::Halo(cmap)}, 

        // {.column = Columns::CAGE_4, .slot = 1, .pattern = new Mid::SnowflakePatternColumn(cmap)},

        {.column = Columns::CAGE_2, .slot = 0, .pattern = new Max::RadialFadePattern(cmap)},
        {.column = Columns::CAGE_2, .slot = 1, .pattern = new Max::RadialGlitterFadePattern(cmap)},
        {.column = Columns::CAGE_2, .slot = 2, .pattern = new Mid::TakkenChase(cmap)},
        {.column = Columns::CAGE_2, .slot = 3, .pattern = new Max::AngularFadePattern(cmap)}, 
        {.column = Columns::CAGE_2, .slot = 4, .pattern = new Min::GrowingCirclesPattern(map)},
        {.column = Columns::CAGE_2, .slot = 5, .pattern = new Min::SpiralPattern(cmap)},
        {.column = Columns::CAGE_2, .slot = 6, .pattern = new Min::SegmentChasePattern()},
        {.column = Columns::CAGE_2, .slot = 7, .pattern = new LedPatterns::FadeFromRandom()},


        {.column = Columns::CAGE_3, .slot = 1, .pattern = new Mid::Lighthouse(cmap)},
        {.column = Columns::CAGE_3, .slot = 2, .pattern = new Max::GrowingStrobePattern(cmap)}, 
        {.column = Columns::CAGE_3, .slot = 3, .pattern = new Hi::DotBeatPattern(smap)}, 
        {.column = Columns::CAGE_3, .slot = 4, .pattern = new Low::GrowShrink(cmap)},
        {.column = Columns::CAGE_3, .slot = 5, .pattern = new Hi::XY(map)}, 
        {.column = Columns::CAGE_3, .slot = 6, .pattern = new Low::RotatingRingsPattern(cmap)}, 
        {.column = Columns::CAGE_3, .slot = 7, .pattern = new Low::OnBeatColumnChaseUpPattern(smap)}, 

        // {.column = Columns::CAGE_4, .slot = 0, .pattern = new Max::AngularFadePattern(cmap)}, //duplicate van cage 2
        {.column = Columns::CAGE_4, .slot = 1, .pattern = new Low::GlowPulsePattern(map)},
        {.column = Columns::CAGE_4, .slot = 2, .pattern = new Min::GlowPulsePattern()},
        {.column = Columns::CAGE_4, .slot = 3, .pattern = new Mid::DoubleFlash(cmap)}, 
        {.column = Columns::CAGE_4, .slot = 4, .pattern = new Min::LineLaunch(map)},
        {.column = Columns::CAGE_4, .slot = 5, .pattern = new LedPatterns::SegmentGlitchPattern()},
        {.column = Columns::CAGE_4, .slot = 6, .pattern = new Flash::FlashesPattern()},
        {.column = Columns::CAGE_4, .slot = 7, .pattern = new Flash::StrobePattern()},

        {.column = Columns::CAGE_MASK, .slot = 0, .pattern = new MaskPatterns::SinChaseMaskPattern()},
        {.column = Columns::CAGE_MASK, .slot = 1, .pattern = new MaskPatterns::SinChaseMaskPattern()},
        {.column = Columns::CAGE_MASK, .slot = 2, .pattern = new MaskPatterns::GlowPulseMaskPattern()},
        {.column = Columns::CAGE_MASK, .slot = 3, .pattern = new CageMaskPatterns::GradientMaskPattern(map, true)},
        {.column = Columns::CAGE_MASK, .slot = 4, .pattern = new CageMaskPatterns::GradientMaskPattern(map, false)},

        {.column = Columns::CAGE_FLASH, .slot = 0, .pattern = new Flash::FlashesPattern()},
        {.column = Columns::CAGE_FLASH, .slot = 1, .pattern = new Flash::SquareGlitchPattern(map)},
        {.column = Columns::CAGE_FLASH, .slot = 2, .pattern = new Flash::PixelGlitchPattern()},
        {.column = Columns::CAGE_FLASH, .slot = 3, .pattern = new Max::GrowingStrobePattern(cmap)},
        {.column = Columns::CAGE_FLASH, .slot = 4, .pattern = new Flash::StrobeHighlightPattern()},
        {.column = Columns::CAGE_FLASH, .slot = 5, .pattern = new Flash::StrobePattern()},
        {.column = Columns::CAGE_FLASH, .slot = 6, .pattern = new Flash::FadingNoisePattern()},
        {.column = Columns::CAGE_FLASH, .slot = 7, .pattern = new LedPatterns::SegmentGlitchPattern()},

        {.column = Columns::BUTTONS, .slot = 0, .pattern = new Buttons::ButtonPressedPattern(map, 0)},
        {.column = Columns::BUTTONS, .slot = 1, .pattern = new Buttons::ButtonPressedPattern(map, 1)},
        {.column = Columns::BUTTONS, .slot = 2, .pattern = new Buttons::ButtonPressedPattern(map, 2)},
        {.column = Columns::BUTTONS, .slot = 3, .pattern = new Buttons::ButtonPressedPattern(map, 3)},
        {.column = Columns::BUTTONS, .slot = 4, .pattern = new Buttons::ButtonPressedPattern(map, 4)},
        {.column = Columns::BUTTONS, .slot = 5, .pattern = new Buttons::ButtonPressedPattern(map, 5)},

        //button sync patterns
        {.column = Columns::BUTTONS_EFFECT, .slot = 0, .pattern = new Buttons::SyncToMeasurePattern(smap)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 1, .pattern = new Buttons::SyncToMeasurePattern(smap)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 2, .pattern = new Buttons::SyncToMeasurePattern(smap)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 3, .pattern = new Buttons::SyncToMeasurePattern(smap)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 4, .pattern = new Buttons::SyncToMeasurePattern(smap)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 5, .pattern = new Buttons::SyncToMeasurePattern(smap)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 6, .pattern = new Buttons::SyncToMeasurePattern(smap)},

        //button effect patterns
        {.column = Columns::BUTTONS_EFFECT, .slot = 0, .pattern = new Buttons::TimedAnimate(new Flash::FadingNoisePattern(),1000)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 1, .pattern = new Buttons::TimedAnimate(new Flash::StrobeFadePattern(smap),2000)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 6, .pattern = new Buttons::TimedAnimate(new Low::VerticallyIsolated(cmap),5000)},

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

        {.column = Columns::DEBUG, .slot = 0, .pattern = new TestPatterns::DistributionPattern(distribution)},
        {.column = Columns::DEBUG, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
        {.column = Columns::DEBUG, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
        {.column = Columns::DEBUG, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
        {.column = Columns::DEBUG, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
        {.column = Columns::DEBUG, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
        {.column = Columns::DEBUG, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
        {.column = Columns::DEBUG, .slot = 7, .pattern = new TestPatterns::Gamma(10)},

    };

    auto input = new ControlHubInput<RGBA>(map->size(), &hyp->hub, patterns);

    distributeAndMonitor3d<GBR, RGBA>(
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
        {"hyperslave4.local", 9611, 8 * 60},
        {"hyperslave4.local", 9612, 8 * 60},
        {"hyperslave4.local", 9613, 8 * 60},
        {"hyperslave4.local", 9614, 8 * 60},
    };

    std::vector<ControlHubInput<RGBA>::SlotPattern> patterns = {
        {
            .column = WINGS_1,
            .slot = 0,
            .pattern = new LedPatterns::PalettePattern(0, "Primary"),
        },
        {
            .column = WINGS_1,
            .slot = 1,
            .pattern = new LedPatterns::PalettePattern(1, "Secondary"),
        },
        {.column = Columns::WINGS_1, .slot = 2, .pattern = new Wings::FadeFromCenter()},
        {.column = Columns::WINGS_1, .slot = 3, .pattern = new LedPatterns::FadeFromRandom()},
        {
            .column = Columns::WINGS_1,
            .slot = 4,
            .pattern = new Mapped2dPatterns::RadialSaw(pmap),
        },
        {.column = Columns::WINGS_1, .slot = 5, .pattern = new Mapped2dPatterns::HorizontalGradientPattern(map)},
        {.column = Columns::WINGS_1, .slot = 6, .pattern = new Mapped2dPatterns::RadialGlitterFadePattern(pmap)},
        {.column = Columns::WINGS_1, .slot = 7, .pattern = new Mapped2dPatterns::RadialFadePattern(pmap)},

        {.column = Columns::WINGS_2, .slot = 0, .pattern = new LedPatterns::SinChasePattern(60), .indexMap = zigzag},
        // {.column = Columns::WINGS_2, .slot = 1, .pattern = new LedPatterns::Fireworks()},
        {.column = Columns::WINGS_2, .slot = 2, .pattern = new LedPatterns::RibbenClivePattern<Glow>(10000, 1, 0.15)},
        {.column = Columns::WINGS_2, .slot = 3, .pattern = new LedPatterns::ClivePattern<SawDown>(32, 1000, 1, 0.1)},
        {.column = Columns::WINGS_2, .slot = 4, .pattern = new LedPatterns::ClivePattern<SoftPWM>(32, 1000, 1, 0.5)},
        {.column = Columns::WINGS_2, .slot = 5, .pattern = new LedPatterns::ClivePattern<Glow>(32, 1000, 1, 0.5)},
        {.column = Columns::WINGS_2, .slot = 6, .pattern = new LedPatterns::BarChase(60), .indexMap = zigzag},
        {.column = Columns::WINGS_2, .slot = 7, .pattern = new LedPatterns::GradientChasePattern(), .indexMap = zigzag},

        {
            .column = Columns::WINGS_3,
            .slot = 0,
            .pattern = new Mapped2dPatterns::SpiralPattern(pmap),
        },
        {.column = Columns::WINGS_3, .slot = 1, .pattern = new LedPatterns::SideWave(60), .indexMap = zigzag},
        {.column = Columns::WINGS_3, .slot = 2, .pattern = new Mapped2dPatterns::Lighthouse(pmap)},
        {
            .column = Columns::WINGS_3,
            .slot = 3,
            .pattern = new Mapped2dPatterns::GrowingCirclesPattern(map),
        },
        {
            .column = Columns::WINGS_3,
            .slot = 4,
            .pattern = new Mapped2dPatterns::HorizontalSin(pmap),
        },
        {
            .column = Columns::WINGS_3,
            .slot = 5,
            .pattern = new Mapped2dPatterns::HorizontalSaw(pmap),
        },
        {
            .column = Columns::WINGS_3,
            .slot = 6,
            .pattern = new Mapped2dPatterns::DotBeatPattern(pmap),
        },
        {.column = Columns::WINGS_3, .slot = 7, .pattern = new Mapped2dPatterns::AngularFadePattern(pmap)},

        {.column = Columns::WINGS_4, .slot = 0, .pattern = new Wings::XY(map)},
        {
            .column = Columns::WINGS_4,
            .slot = 1,
            .pattern = new LedPatterns::FlashesPattern(),
        },

        {
            .column = Columns::WINGS_4,
            .slot = 2,
            .pattern = new LedPatterns::SegmentChasePattern(),
        },
        {
            .column = Columns::WINGS_4,
            .slot = 3,
            .pattern = new LedPatterns::GlowPulsePattern(),
        },
        {
            .column = Columns::WINGS_4,
            .slot = 4,
            .pattern = new LedPatterns::StrobePattern(),
        },
        {
            .column = Columns::WINGS_4,
            .slot = 5,
            .pattern = new LedPatterns::PixelGlitchPattern(),
        },
        {.column = Columns::WINGS_4, .slot = 6, .pattern = new LedPatterns::SegmentGlitchPattern()},
        {
            .column = Columns::WINGS_4,
            .slot = 7,
            .pattern = new Mapped2dPatterns::LineLaunch(map),
        },

        {.column = Columns::WINGS_MASK, .slot = 0, .pattern = new MaskPatterns::SinChaseMaskPattern()},
        {.column = Columns::WINGS_MASK, .slot = 1, .pattern = new MaskPatterns::SinChaseMaskPattern(), .indexMap = zigzag},
        {.column = Columns::WINGS_MASK, .slot = 2, .pattern = new MaskPatterns::GlowPulseMaskPattern()},
        {.column = Columns::WINGS_MASK, .slot = 3, .pattern = new MaskPatterns::SegmentGradientMaskPattern(60, true), .indexMap = zigzag},

        {
            .column = Columns::WINGS_FLASH,
            .slot = 0,
            .pattern = new LedPatterns::FadingNoisePattern(),
        },
        {
            .column = Columns::WINGS_FLASH,
            .slot = 1,
            .pattern = new LedPatterns::FlashesPattern(),
        },
        {
            .column = Columns::WINGS_FLASH,
            .slot = 2,
            .pattern = new LedPatterns::StrobePattern(),
        },
        {
            .column = Columns::WINGS_FLASH,
            .slot = 3,
            .pattern = new LedPatterns::StrobeHighlightPattern(),
        },
        {.column = Columns::WINGS_FLASH, .slot = 4, .pattern = new LedPatterns::SegmentGlitchPattern()},

        {.column = Columns::DEBUG, .slot = 0, .pattern = new TestPatterns::DistributionPattern(distribution, 60)},
        {.column = Columns::DEBUG, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
        {.column = Columns::DEBUG, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
        {.column = Columns::DEBUG, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
        {.column = Columns::DEBUG, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
        {.column = Columns::DEBUG, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
        {.column = Columns::DEBUG, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
        {.column = Columns::DEBUG, .slot = 7, .pattern = new TestPatterns::Gamma(10)},

        {.column = Columns::BUTTONS_EFFECT, .slot = 4, .pattern = new Buttons::TimedAnimate(new Mapped2dPatterns::HorizontalGradientPattern(map), 5000)},
        {.column = Columns::BUTTONS_EFFECT, .slot = 4, .pattern = new Buttons::TimedAnimate(new MaskPatterns::SinChaseMaskPattern(), 5000+1000), .indexMap = zigzag},
        {.column = Columns::BUTTONS_EFFECT, .slot = 4, .pattern = new Buttons::TimedAnimate(new MaskPatterns::SegmentGradientMaskPattern(60, true), 5000+1000), .indexMap = zigzag},
            
    };

    auto input = new ControlHubInput<RGBA>(map->size(), &hyp->hub, patterns);

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
    PixelMap::Polar *pmap = new PixelMap::Polar(map->toPolarRotate90());

    Distribution distribution = {
        {"hyperslave3.local", 9611, wingSize},
        {"hyperslave3.local", 9612, wingSize},
        {"hyperslave3.local", 9613, wingSize},
        {"hyperslave3.local", 9614, wingSize},
        {"hyperslave3.local", 9615, wingSize},
        {"hyperslave3.local", 9616, wingSize},
        {"hyperslave3.local", 9617, 100},
    };

    auto input = new ControlHubInput<RGBA>(

        map->size(),
        &hyp->hub,
        {
            {
                .column = VULTURE,
                .slot = 0,
                .pattern = new LedPatterns::PalettePattern(0, "Primary"),
            },
            {.column = Columns::VULTURE, .slot = 1, .pattern = new Mapped2dPatterns::HorizontalGradientPattern(map)},
            {.column = Columns::VULTURE, .slot = 2, .pattern = new Mapped2dPatterns::Lighthouse(pmap)},
            {.column = Columns::VULTURE, .slot = 3, .pattern = new Mapped2dPatterns::RadialGlitterFadePattern(pmap)},
            {.column = Columns::VULTURE, .slot = 4, .pattern = new Mapped2dPatterns::RadialFadePattern(pmap)},
            {
                .column = Columns::VULTURE,
                .slot = 5,
                .pattern = new LedPatterns::GlowPulsePattern(),
            },
            {
                .column = Columns::VULTURE,
                .slot = 6,
                .pattern = new LedPatterns::StrobePattern(),
            },
            {
                .column = Columns::VULTURE,
                .slot = 7,
                .pattern = new LedPatterns::PixelGlitchPattern(),
            },

            {.column = Columns::DEBUG, .slot = 0, .pattern = new TestPatterns::DistributionPattern(distribution, 100)},
            {.column = Columns::DEBUG, .slot = 1, .pattern = new TestPatterns::OneColor(RGB(255, 0, 0), "Red")},
            {.column = Columns::DEBUG, .slot = 2, .pattern = new TestPatterns::OneColor(RGB(0, 255, 0), "Green")},
            {.column = Columns::DEBUG, .slot = 3, .pattern = new TestPatterns::OneColor(RGB(0, 0, 255), "Blue")},
            {.column = Columns::DEBUG, .slot = 4, .pattern = new TestPatterns::OneColor(RGB(255, 255, 255), "White")},
            {.column = Columns::DEBUG, .slot = 5, .pattern = new TestPatterns::OneColor(RGB(127, 127, 127), "White 50%")},
            {.column = Columns::DEBUG, .slot = 6, .pattern = new TestPatterns::Palette(10, 1)},
            {.column = Columns::DEBUG, .slot = 7, .pattern = new TestPatterns::Gamma(10)},
        });

    distributeAndMonitor<BGR, RGBA>(
        hyp, input, map, distribution, ledBarLut);

    hyp->hub.setColumnName(8, "Wings");
}

void addDMXPipe(Hyperion *hyp)
{
    PixelMap *map = new PixelMap(combineMaps({
        PixelMap({{.x = -0.02, .y = 0}, {.x = 0.02, .y = 0}}), // eyes

        PixelMap({{.x = 0, .y = 0.85}}),     // motor wings
        gridMap(4, 1, 0.1, 0.1, -0.4, 0.85), // fire 1, marten
        PixelMap({{.x = -0.4, .y = 0.9}}),   // fire 2, joel
        gridMap(4, 1, 0.1, 0.1, 0.4, 0.85),  // pinspots
        {{.x = -0, .y = 0.5}},               //fog
    }));

    // channel mapping:
    //  1-2: eyes
    //  3: wings motor
    //  4-5, 6-7: fire
    //  8-11: pinspots
    //  12: fog

    Distribution distribution = {{"hyperslave4.local", 9619, (int)map->size()}};

    auto inputBase = new ControlHubInput<Monochrome>(
        map->size()-1,
        &hyp->hub,
        {
            {.column = Columns::EFFECTS, .slot = 3, .pattern = new MonochromePatterns::StaticPattern("Eyes", {{.channel = 0}, {.channel = 1}})},
            {.column = Columns::EFFECTS, .slot = 4, .pattern = new MonochromePatterns::StaticPattern("Wings motor", {{.channel = 2}})},
            {.column = Columns::EFFECTS, .slot = 5, .pattern = new MonochromePatterns::StaticPattern("Fire", {{.channel = 3}, {.channel = 4}, {.channel = 5}, {.channel = 6}})},
            {.column = Columns::EFFECTS, .slot = 6, .pattern = new MonochromePatterns::StaticPattern("Fire big", {{.channel = 7}} )},

            {.column = Columns::BUTTONS_EFFECT, .slot = 2, .pattern = new Buttons::TimedAnimate(new MonochromePatterns::StaticPattern("Fire ",  {{.channel = 3}, {.channel = 4}, {.channel = 5}, {.channel = 6}}),1000)},
            {.column = Columns::BUTTONS_EFFECT, .slot = 3, .pattern = new Buttons::TimedAnimate(new MonochromePatterns::StaticPattern("Fire big", {{.channel = 7}}),1000)},
            {.column = Columns::BUTTONS_EFFECT, .slot = 5, .pattern = new Buttons::TimedAnimate(new MonochromePatterns::StaticPattern("Eyes", {{.channel = 0},{.channel = 1}}, 200, 1000),10000)},
        });

    auto inputPinspots = new ControlHubInput<Monochrome>(
        4,
        &hyp->hub,
        {
            {.column = Columns::EFFECTS, .slot = 7, .pattern = new MonochromePatterns::OnPattern(255, "Pinspots on")},
            {.column = Columns::EFFECTS, .slot = 8, .pattern = new MonochromePatterns::GlowPattern()},

            {.column = Columns::BUTTONS_EFFECT, .slot = 4, .pattern = new Buttons::TimedAnimate(new MonochromePatterns::OnPattern(255, "Pinspots on"),1000)},
            
        });

    // auto inputMotor = new ControlHubInput<Monochrome>(
    //     1,
    //     &hyp->hub,
    //     {
    //         {.column = Columns::EFFECTS, .slot = 5, .pattern = new MonochromePatterns::OnPattern(255, "Wings motor")},
    //     });

    // auto combined = new CombinedInput({
    //     {.input = inputEyes, .offset=0},
    //     {.input = inputPinspots, .offset=2},
    //     {.input = inputMotor, .offset=6},
    // }, 100);

    auto inputFog = new ControlHubInput<Monochrome>(
        1,
        &hyp->hub,
        {
            {.column = Columns::FOG, .slot = 0, .pattern = new MonochromePatterns::OnPattern(255, "Fog"), .noMasterDim=true},
        
            {.column = Columns::FOG, .slot = 1, .pattern = new MonochromePatterns::IntervalPattern(4, 64, "Interval 4/64 beats (minst)"), .noMasterDim=true},
            {.column = Columns::FOG, .slot = 4, .pattern = new MonochromePatterns::IntervalPattern(8, 64, "Interval 8/64 beats"), .noMasterDim=true},
            {.column = Columns::FOG, .slot = 2, .pattern = new MonochromePatterns::IntervalPattern(4, 32, "Interval 4/32 beats"), .noMasterDim=true},
            {.column = Columns::FOG, .slot = 3, .pattern = new MonochromePatterns::IntervalPattern(4, 16, "Interval 4/16 beats"), .noMasterDim=true},
            {.column = Columns::FOG, .slot = 5, .pattern = new MonochromePatterns::IntervalPattern(8, 16, "Interval 8/16 beats (meest)"), .noMasterDim=true},  
        }
    );

    auto combined = new CombinedInput({
                                          {.input = inputBase, .offset = 0},
                                          {.input = inputPinspots, .offset = 8},
                                          {.input = inputFog, .offset = 11},
                                      },
                                      100);

    std::vector<InputSlicer::Slice> slices = {
        {.start = 0, .length = (int)map->size(), .sync = true},
        {.start = 0, .length = (int)map->size(), .sync = false}};
    auto splitInput = new InputSlicer(combined, slices);
    distribute<Monochrome, Monochrome>(hyp, distribution, splitInput);

    hyp->addPipe(
        new ConvertPipe<Monochrome, RGB>(
            splitInput->getInput(slices.size() - 1),
            new MonitorOutput(&hyp->webServer, map, 60)));
}

void addLightningPipe(Hyperion *hyp)
{
    PixelMap *map = new PixelMap(gridMap(8, 1, 0.1, 0.1, 0, 0.7));

    Distribution distribution = {{"hyperslave4.local", 9620, (int)map->size()}};

    auto input = new ControlHubInput<Monochrome>(
        map->size(),
        &hyp->hub,
        {
            {.column = Columns::EFFECTS, .slot = 0, .pattern = new MonochromePatterns::FastStrobePattern2()},
            {.column = Columns::EFFECTS, .slot = 1, .pattern = new MonochromePatterns::GlitchPattern()},
            {.column = Columns::EFFECTS, .slot = 2, .pattern = new MonochromePatterns::BeatShakePattern()},
        });

    distributeAndMonitor<Monochrome12, Monochrome>(
        hyp, input, map, distribution);
}

int main()
{
    auto hyp = new Hyperion();
    hyp->setMidiControllerFactory(new ButtonMidiControllerFactory());

    setupPaletteColumn(hyp, Columns::PALETTE);
    addCagePipe(hyp);
    addWingsPipe(hyp);
    addVulturePipe(hyp);
    addDMXPipe(hyp);
    addLightningPipe(hyp);

    hyp->hub.setColumnName(Columns::PALETTE, "Palette");
    hyp->hub.setColumnName(Columns::CAGE_1, "Cage");
    hyp->hub.setColumnName(Columns::CAGE_2, "Cage");
    hyp->hub.setColumnName(Columns::CAGE_3, "Cage");
    hyp->hub.setColumnName(Columns::CAGE_4, "Cage");
    hyp->hub.setColumnName(Columns::CAGE_MASK, "Mask");
    hyp->hub.setColumnName(Columns::CAGE_FLASH, "Flash");
    hyp->hub.setColumnName(Columns::EFFECTS, "FX");
    hyp->hub.setColumnName(Columns::WINGS_1, "Wings");
    hyp->hub.setColumnName(Columns::WINGS_2, "Wings");
    hyp->hub.setColumnName(Columns::WINGS_3, "Wings");
    hyp->hub.setColumnName(Columns::WINGS_4, "Wings");
    hyp->hub.setColumnName(Columns::WINGS_MASK, "Mask");
    hyp->hub.setColumnName(Columns::WINGS_FLASH, "Flash");
    hyp->hub.setColumnName(Columns::VULTURE, "Vulture");
    hyp->hub.setColumnName(Columns::FOG, "Fog");
    hyp->hub.setColumnName(Columns::BUTTONS, "Buttons");
    hyp->hub.setColumnName(Columns::BUTTONS_EFFECT, "Buttons FX");
    hyp->hub.setColumnName(Columns::DEBUG, "Debug");

    hyp->hub.setFlashColumn(Columns::CAGE_FLASH);
    hyp->hub.setFlashColumn(Columns::WINGS_FLASH);
    hyp->hub.setFlashColumn(Columns::EFFECTS);

    hyp->hub.setForcedSelection(Columns::BUTTONS_EFFECT);
    hyp->hub.buttonPressed(Columns::BUTTONS_EFFECT, 0);

    hyp->hub.setFlashColumn(Columns::FOG, false, true);
    hyp->hub.findSlot(Columns::FOG, 0)->releaseColumn = true;
    hyp->hub.findSlot(Columns::FOG, 0)->flash = true;
    
    hyp->start();

    Tempo::AddSource(new ConstantTempo(120));
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
