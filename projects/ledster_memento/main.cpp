#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/inputs/fallbackInput.hpp"
#include "core/distribution/inputs/switchableInput.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/patternCycleInput.hpp"
#include "mapping/ledsterMap.hpp"
#include "mapping/ledsterMap3d.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "palettes.hpp"
#include "patterns.hpp"
#include "distribution/inputs/controlHubInput.hpp"
// #include "dynamicPipe.hpp"
#include <vector>

#if ESP_PLATFORM
#include "wifi-ap.hpp"
#else
void setup_wifi() {}
#endif

LUT *ledsterLut = new ColourCorrectionLUT(1.5, 255, 255, 255, 240);

auto cLedsterMap3d = ledsterMap3d.toCylindricalXZ();

std::vector<PaletteColumn::Palette> palettes = std::vector<PaletteColumn::Palette>({
    pinkSunset,
    sunset8,
    heatmap2,
    sunset2,
    sunset6,
    plumBath,
    redSalvation});

auto hyp = new Hyperion();
int autoColumn=2;

int main()
{
    setup_wifi();
    char hostname[14];
    snprintf(hostname,14,"hyperion-%x%x",Ethernet::getMac().octets[0],Ethernet::getMac().octets[1]);
    Network::setHostName(hostname);
    Log::info("HYPER","Hostname: %s",hostname);

    Tempo::AddSource(new ConstantTempo(120));

    auto inputUdp = new UDPInput(9601);

    std::vector<Pattern<RGBA>*> patterns = {
        new Patterns::Lighthouse(&cLedsterMap3d),
        new Patterns::PetalChase(&cLedsterMap3d),
        new Patterns::SnowflakePatternLedster(),
        new Patterns::SnakePattern(),
        new Patterns::XY(&ledsterMap3d),
        new Patterns::RibbenClivePattern<Glow>(10000, 1, 0.15),
        new Patterns::GrowingCirclesPattern(&ledsterMap3d),
        new Patterns::SpiralPattern(&cLedsterMap3d),
        new Patterns::SegmentChasePattern(),
        new Patterns::GlowPulsePattern(),
        new Patterns::PetalRotatePattern(),
    };

    auto inputAuto = new PatternCycleInput<RGBA>(ledsterMap3d.size(), patterns,
    3 * 60 * 1000);

    auto inputControlled = new ControlHubInput<RGBA>(ledsterMap3d.size(),&hyp->hub,1,patterns);

    auto input = new FallbackInput(
        inputUdp,
        new SwitchableInput(
            inputAuto,
            inputControlled,
            [](){return hyp->hub.findSlot(autoColumn,0)->activated;}
        )
    );

    hyp->hub.expandTo(autoColumn,0);
    hyp->hub.setColumnName(autoColumn,"Palette");
    hyp->hub.setColumnName(autoColumn,"Patterns");
    hyp->hub.setColumnName(autoColumn,"Auto");
    hyp->hub.findSlot(autoColumn,0)->name="Auto";
    hyp->hub.buttonPressed(autoColumn,0);
    hyp->hub.subscribe(new PaletteColumn(&hyp->hub,0,0,palettes));
    hyp->hub.setFlashColumn(0, false, true);
    hyp->hub.setForcedSelection(0);

#if ESP_PLATFORM
    // ledster big
    // hyp->addPipe(new ConvertPipe<RGBA, GRB>(
    //     input,
    //     new NeopixelOutput(1),
    //     //new SpiOutput(0,1),
    //     ledsterLut));
    // hyp->start(minimal);

    //ledster mini
    int size1 = 271 * sizeof(RGBA);
    int size2 = 210 * sizeof(RGBA);
    auto splitInput = new InputSplitter(input, {size1, size2}, true);

    //TOOD use dynamicPipe to convert form RGB to GRB, instead of RGBA to GRB when data is coming from UDP
    hyp->addPipe(new ConvertPipe<RGBA, GRB>(
        splitInput->getInput(0),
        new NeopixelOutput(6),
        ledsterLut));
    hyp->addPipe(new ConvertPipe<RGBA, GRB>(
        splitInput->getInput(1),
        new NeopixelOutput(8),
        ledsterLut));

  hyp->start({
      .network = false,
      .rotary = false,
      .display = false,
      .midi = false,
      .tempo = false,
      .web = true,
  });
#else
    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new MonitorOutput(&hyp->webServer, &ledsterMap)));
    hyp->start();
#endif

    int paletteColumnIndex = 0;
    while (1)
    {
        inputAuto->params.gradient = palettes[paletteColumnIndex].gradient;
        inputAuto->params.primaryColour = palettes[paletteColumnIndex].primary;
        inputAuto->params.secondaryColour = palettes[paletteColumnIndex].secondary;
        inputAuto->params.highlightColour = palettes[paletteColumnIndex].highlight;

        paletteColumnIndex++;
        if (paletteColumnIndex >= palettes.size())
            paletteColumnIndex = 0;
        Thread::sleep(4 * 60 * 1000);
    }
}
