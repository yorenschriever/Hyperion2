#pragma once

#include "../fpsCounter.hpp"
#include "../interfaces.hpp"
#include <inttypes.h>

// Derive from this class when you are going to write new inputs. You should at least implement
// process(), and you can implement initialize(), if there is some setup to do. Putting the
// initialization code in initialize() instead of the constructor makes you code easier to debug,
// because at that point the logging system is guaranteed to be initialized.
class BaseInput : public IInput
{

public:
    virtual FPSCounter *getFpsCounter() override
    {
        return &fpsCounter;
    }

    virtual void initialize() override {}

protected:
    FPSCounter fpsCounter;
};
