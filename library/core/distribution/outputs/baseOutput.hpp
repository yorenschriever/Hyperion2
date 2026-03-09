#pragma once

#include <inttypes.h>
#include "../interfaces.hpp"

class Hyperion;

//You should derive from this class when writing your own output
class BaseOutput : public ISink
{

public:
    bool ready() override = 0;
    void process(const Buffer&) override = 0;
};
