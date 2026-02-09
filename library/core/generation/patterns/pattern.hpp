#pragma once

#include "core/colors.h"
#include "helpers/fade.h"
#include "helpers/interval.h"
#include "helpers/lfo.h"
#include "helpers/params.h"
#include "helpers/permute.h"
#include "helpers/timeline.h"
#include "helpers/transition.h"
#include "helpers/watcher.h"
#include <inttypes.h>
#include <string>

template <class T_COLOR>
class Pattern
{

public:
    // frameBuffer = the buffer to fill. 
    //    contains data underlying layers that you have to mix this pattern into
    //    (only if active is true)
    // width = number of pixels
    // active = is the pattern activated
    virtual void Calculate(T_COLOR *frameBuffer, int width, bool active, Params *params){}

    std::string name; 
};

