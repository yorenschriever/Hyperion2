#pragma once
#include <inttypes.h>
#include "buffer.hpp"

// ISource and ISink are terms used within the system to form chains,
// A chain starts with an ISource, and can optionally contain multiple processor objects,
// and ends with an ISink. 

class ISource 
{
public:
    virtual void initialize() = 0;

    //indicates whether the source has data ready to be processed
    virtual bool ready() = 0;

    virtual const Buffer process() = 0;
};

class ISink
{
public:
    virtual void initialize() =0;

    // Indicates whether the sink is ready to receive data.
    virtual bool ready() = 0;

    virtual void process(const Buffer&) = 0;
};

class IProcessor 
{
public:
    virtual void initialize() =0;
    virtual bool ready() = 0;
    virtual const Buffer process(const Buffer&) = 0;
};