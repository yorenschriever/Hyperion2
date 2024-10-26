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
    hyp->addPipe(new ConvertPipe<MotorPosition, MotorPositionOut>(
        input, 
        new WebsocketOutput(&hyp->webServer, "/ws/windows", 60)
        ));

    // hyp->hub.setForcedSelection(1, true);
    hyp->hub.setFlashColumn(1,false,true);

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
