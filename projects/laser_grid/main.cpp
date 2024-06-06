#include "core/hyperion.hpp"
#include "mapping/ledMap.hpp"
#include "patterns.hpp"

// LUT *ledsterLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

// auto cLedsterMap3d = ledsterMap3d.toCylindricalXZ();

// int autoColumn=2;

int main()
{
    auto hyp = new Hyperion();

    Tempo::AddSource(new ConstantTempo(120));

    std::vector<Pattern<Monochrome> *> patterns = {
        new Patterns::OnPattern(255),
        new Patterns::SinPattern(),
        new Patterns::GlowPattern(),
        // new Patterns::SnowflakePatternLedster(),
        // new Patterns::SnakePattern(),
        // new Patterns::XY(&ledsterMap3d),
        // new Patterns::RibbenClivePattern<Glow>(10000, 1, 0.15),
        // new Patterns::GrowingCirclesPattern(&ledsterMap3d),
        // new Patterns::SpiralPattern(&cLedsterMap3d),
        // new Patterns::SegmentChasePattern(),
        // new Patterns::GlowPulsePattern(),
        // new Patterns::PetalRotatePattern(),
    };

    // auto inputAuto = new PatternCycleInput<Monochrome>(ledMap1.size(), patterns,
    // 3 * 60 * 1000);

    auto inputControlled = new ControlHubInput<Monochrome>(ledMap1.size(), &hyp->hub, 1, patterns);

    // auto input =
    //     new SwitchableInput(
    //         inputAuto,
    //         inputControlled,
    //         [](){return hyp->hub.findSlot(autoColumn,0)->activated;}
    //     );

    auto input = inputControlled;

    // hyp->hub.expandTo(autoColumn,0);
    // hyp->hub.setColumnName(autoColumn,"Palette");
    // hyp->hub.setColumnName(autoColumn,"Patterns");
    // hyp->hub.setColumnName(autoColumn,"Auto");
    // hyp->hub.findSlot(autoColumn,0)->name="Auto";
    // hyp->hub.buttonPressed(autoColumn,0);
    // hyp->hub.subscribe(new PaletteColumn(&hyp->hub,0,0,palettes));
    // hyp->hub.setFlashColumn(0, false, true);
    // hyp->hub.setForcedSelection(0);

    auto splitInput = new InputSlicer(
        input,
        {
            {0, 10, true},
            {10, 10, true},
            {20, 10, true},
            {30, 10, true},
            {0, 40, false},
        });

    std::string ips[] = {
        "192.168.1.175",
        "192.168.1.176",
        "192.168.1.177",
        "192.168.1.178"};

    for (int i = 0; i < 4; i++)
        hyp->addPipe(
            new Pipe(
                splitInput->getInput(i),
                new UDPOutput(ips[i].c_str(), 9601,60)));

    hyp->addPipe(new ConvertPipe<Monochrome, RGB>(
        splitInput->getInput(4),
        new MonitorOutput(&hyp->webServer, &ledMap1)));
    hyp->start();

    while (1)
    {
        Thread::sleep(4 * 60 * 1000);
    }
}
