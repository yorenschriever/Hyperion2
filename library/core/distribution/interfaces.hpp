#pragma once
#include "fpsCounter.hpp"
#include <inttypes.h>
#include "buffer.hpp"

// ISource and ISink are terms used within the system to form chains,
// A chain starts with an ISource, and can optionally contain multiple converter objects,
// and ends with an ISink. 

class ISource 
{
public:
    virtual void initialize() = 0;

    //indicates whether the source has data ready to be processed
    virtual bool ready() = 0;

    virtual Buffer process() = 0;

    virtual FPSCounter *getFpsCounter() = 0;
};

class ISink
{
public:
    virtual void initialize() =0;

    // Indicates whether the sink is ready to receive data.
    virtual bool ready() = 0;

    virtual void process(const Buffer&) = 0;

    virtual FPSCounter *getFpsCounter() = 0;
};

class IConverter 
{
public:
    virtual void initialize() =0;
    virtual bool ready() = 0;
    virtual Buffer process(const Buffer&) = 0;
};