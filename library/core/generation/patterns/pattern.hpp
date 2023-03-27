#pragma once

#include "core/colours.h"
#include "helpers/fade.h"
#include "helpers/interval.h"
#include "helpers/lfo.h"
#include "helpers/params.h"
#include "helpers/permute.h"
#include "helpers/timeline.h"
#include "helpers/transition.h"
#include "helpers/watcher.h"
#include <inttypes.h>

template <class T_COLOUR>
class Pattern
{

public:
    // override this method if you want to complex initialization behaviour.
    // do not put such behaviour in the constructor
    virtual void Initialize() {}

    // frameBuffer = the buffer to fill. 
    //    contains data underlying layers that you have to mix this pattern into
    //    (only if active is true)
    // width = number of pixels
    // active = is the pattern activated
    virtual void Calculate(T_COLOUR *frameBuffer, int width, bool active, Params *params){}
};

