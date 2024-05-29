#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/hyperion.hpp"
#include "distribution/inputs/patternInput.hpp"
#include "mapping/crossMap.hpp"
// #include "palettes.hpp"
#include "patterns.hpp"
#include <vector>

int main()
{
    auto hyp = new Hyperion();

    Tempo::AddSource(new ConstantTempo(120));

    auto input = new PatternInput<RGBA>(
        crossMap.size(),
        new Patterns::Cross(&crossMap));

    input->params.palette = &tunnel;

    hyp->addPipe(new ConvertPipe<RGBA, RGB>(
        input,
        new MonitorOutput(&hyp->webServer, &crossMap)));

    hyp->start();

    while (1)
        Thread::sleep(60 * 1000);
}
