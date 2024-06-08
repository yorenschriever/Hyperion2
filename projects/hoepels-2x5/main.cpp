#include "core/hyperion.hpp"
#include "mapping/ledMap.hpp"
#include "patterns.hpp"
#include "masks.hpp"

void addPaletteColumn(Hyperion *hyp);

int main()
{
    auto hyp = new Hyperion();

    addPaletteColumn(hyp);

    Tempo::AddSource(new ConstantTempo(120));

    std::vector<Pattern<RGBA> *> patterns = {
        new Hoepels::MonochromePattern(),
        new Hoepels::StereochromePattern(),
        new Hoepels::GradientPattern(),

        // rainbow
        // new Hoepels::Rainbow(),
        // new Hoepels::Rainbow2(),
        // new Hoepels::Rainbow3(),

        // sin
        new Hoepels::HoepelsTransition(),
        new Hoepels::SinStripPattern(),
        new Hoepels::SinStripPattern2(),

        // beat
        new Hoepels::OnbeatFadeAllPattern(),
        new Hoepels::OnbeatFadePattern(),
        new Hoepels::FireworkPattern(),

        // chase
        new Hoepels::SinPattern(),
        new Hoepels::AntichasePattern(),
        new Hoepels::ChasePattern(),

        // glow
        new Hoepels::ClivePattern<SinFast>(0, 500, 2000),
        new Hoepels::ClivePattern<SinFast>(1, 10, 2000),
        new Hoepels::GlowPulsePattern(),

        // segment
        new Hoepels::ClivePattern<SoftPWM>(0, 50, 10000, 1, 0.5, 0.1),
        new Hoepels::ClivePattern<SawDown>(1, 25, 1500, 1, 0.1),
        new Hoepels::ClivePattern<SawDown>(0, 50, 1000),

        // flash
        new Hoepels::SlowStrobePattern(),
        new Hoepels::SquareGlitchPattern(),
        new Hoepels::ClivePattern<SawDown>(1, 25, 500, 1, 0.1),
    };

    // auto inputAuto = new PatternCycleInput<Monochrome>(ledMap.size(), patterns,
    // 3 * 60 * 1000);

    // auto inputControlled = new ControlHubInput<RGBA>(ledMap.size(), &hyp->hub, 1, patterns);

    auto input = new ControlHubInput<RGBA>(ledMap.size(), &hyp->hub, {
        {.column = 1, .slot = 0, .pattern = new Hoepels::MonochromePattern()},
        {.column = 1, .slot = 1, .pattern = new Hoepels::StereochromePattern()},
        {.column = 1, .slot = 2, .pattern = new Hoepels::GradientPattern()},

        {.column = 2, .slot = 0, .pattern = new Hoepels::HoepelsTransition()},
        {.column = 2, .slot = 1, .pattern = new Hoepels::SinStripPattern()},
        {.column = 2, .slot = 2, .pattern = new Hoepels::SinStripPattern2()},

        {.column = 3, .slot = 0, .pattern = new Hoepels::OnbeatFadeAllPattern()},
        {.column = 3, .slot = 1, .pattern = new Hoepels::OnbeatFadePattern()},
        {.column = 3, .slot = 2, .pattern = new Hoepels::FireworkPattern()},

        {.column = 4, .slot = 0, .pattern = new Hoepels::SinPattern()},
        {.column = 4, .slot = 1, .pattern = new Hoepels::AntichasePattern()},
        {.column = 4, .slot = 2, .pattern = new Hoepels::ChasePattern()},

        {.column = 5, .slot = 0, .pattern = new Hoepels::ClivePattern<SinFast>(0, 500, 2000)},
        {.column = 5, .slot = 1, .pattern = new Hoepels::ClivePattern<SinFast>(1, 10, 2000)},
        {.column = 5, .slot = 2, .pattern = new Hoepels::GlowPulsePattern()},

        {.column = 6, .slot = 0, .pattern = new Masks::SinChaseMaskPattern()},
        {.column = 6, .slot = 1, .pattern = new Masks::GlowPulseMaskPattern()},

        {.column = 7, .slot = 0, .pattern = new Hoepels::SlowStrobePattern()},
        {.column = 7, .slot = 1, .pattern = new Hoepels::SquareGlitchPattern()},
        {.column = 7, .slot = 2, .pattern = new Hoepels::ClivePattern<SawDown>(1, 25, 500, 1, 0.1)},

    });


    // select first palette
    hyp->hub.buttonPressed(0, 0);
    hyp->hub.setColumnName(0, "Palette");
    hyp->hub.setColumnName(1, "Static");
    hyp->hub.setColumnName(2, "Sin");
    hyp->hub.setColumnName(3, "Beat");
    hyp->hub.setColumnName(4, "Chase");
    hyp->hub.setColumnName(5, "Glow");
    hyp->hub.setColumnName(6, "Mask");
    hyp->hub.setColumnName(7, "Flash");
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setFlashColumn(1, false, true);
    hyp->hub.setForcedSelection(0);


    // auto input =
    //     new SwitchableInput(
    //         inputAuto,
    //         inputControlled,
    //         [](){return hyp->hub.findSlot(autoColumn,0)->activated;}
    //     );

    // auto input = inputControlled;

    const int sz = 100 * sizeof(RGBA);
    auto splitInput = new InputSlicer(
        input,
        {
            {0 * sz, sz, true},
            {1 * sz, sz, true},
            {2 * sz, sz, true},
            {3 * sz, sz, true},
            {4 * sz, sz, true},

            {0, 5 * sz, false},
        });

    std::string ips[] = {
        "192.168.1.222",
        "192.168.1.223",
        "192.168.1.224",
        "192.168.1.226"
        "192.168.1.229",
    };

    for (int i = 0; i < 5; i++)
        hyp->addPipe(
            new ConvertPipe<RGBA, RGB>(
                splitInput->getInput(i),
                new UDPOutput(ips[i].c_str(), 9601, 50)));

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        splitInput->getInput(5),
        new MonitorOutput(&hyp->webServer, &ledMap)));
    hyp->start();

    while (1)
    {
        Thread::sleep(4 * 60 * 1000);
    }
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