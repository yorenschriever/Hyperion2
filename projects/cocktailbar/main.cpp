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
            // new Max::ChevronsPattern(&triangleMap3d),
            // new Max::ChevronsPattern(&triangleMap3d)
        }
    );
    hyp->addPipe(new ConvertPipe<MotorPosition, MotorPosition>(
        input, 
        new WebsocketOutput(&hyp->webServer, "/ws/windows", 60)
        ));

    hyp->start();

    while (1)
        Thread::sleep(1000);
}
