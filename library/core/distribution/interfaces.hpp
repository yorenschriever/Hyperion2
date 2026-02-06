#pragma once
#include "fpsCounter.hpp"
#include <inttypes.h>
#include "buffer.hpp"

// ISource and ISink are terms used within the system to form chains,
// while IInput and IOutput are terms used when connecting systems to each other.
// A chain starts with an ISource, and can optionally contain multiple converter objects,
// and ends with an ISink. An IInput is a source, and an IOutput is a sink.

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

    virtual void process(const Buffer) = 0;

    virtual FPSCounter *getFpsCounter() = 0;
};

// An IInput is an input to the entire system. It is a source where
// pixel data is generated or comes in externally

class IInput : public ISource
{ 
};

// An IOutput is an output to the entire system. It is a sink where
// pixel data leaves the system. Eg to a display, network or hardware port.
class IOutput : public ISink
{
public:
    virtual void clear() = 0;
};

class IConverter 
{
public:
    virtual void initialize() =0;
    virtual bool ready() = 0;
    virtual Buffer process(const Buffer) = 0;
};