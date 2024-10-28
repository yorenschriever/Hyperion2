#include "core/hyperion.hpp"
#include "patterns-window.hpp"
#include "MotorPosition.hpp"

int main()
{
    auto hyp = new Hyperion();

    auto input = new ControlHubInput<MotorPosition>(
        8,
        &hyp->hub,
        1,
        {
            new Window::SinPattern(),
            new Window::SinGapPattern(),
            new Window::FallingPattern(),
        }
    );

    int nbytes = 8 * sizeof(MotorPosition);
    auto split = new InputSlicer(input, {
            {0, nbytes, true},
            {0, nbytes, false},
    });

    hyp->addPipe(new ConvertPipe<MotorPosition, MotorPositionOut>(
        split->getInput(0), 
        new ArtNetOutput("169.254.0.201",0,0,1, 60)
    ));

    hyp->addPipe(new ConvertPipe<MotorPosition, MotorPositionOut>(
        split->getInput(1), 
        new WebsocketOutput(&hyp->webServer, "/ws/windows", 60)
    ));

    // hyp->hub.setForcedSelection(1, true);
    hyp->hub.setFlashColumn(1,false,true);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
