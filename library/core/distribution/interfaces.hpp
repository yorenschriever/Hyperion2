#pragma once
#include "fpsCounter.hpp"
#include <inttypes.h>

// TODO separate files

class IReceiver;
class IInput;
class IOutput;
class IRegisterable;

// IReceiver and ISender are terms used within the system to form chains,
// while IInput and IOutput  are terms used when connecting systems to each other.
// A chain starts with an IInput, and can optionally contain multiple converter objects,
// and ends with an IOutput. An IInput is a sender, and an IOutput is a receiver.
// converter objects are both senders and receivers.

class IRegistrant 
{
public:
    virtual void addInput(IInput*) =0;
    virtual void addOutput(IOutput*) =0;
    virtual void addInitializer(IRegisterable*)=0;;
};

class IRegisterable
{
public:
    virtual void registerToHyperion(IRegistrant *hyp) {} ;
    virtual void initialize() {};
};

// A sender is an object that can send pixel data to a receiver.
// A sender can be attached to an output with setReceiver() to
// form a processing chain.
class ISender : public IRegisterable
{
public:
    virtual void setReceiver(IReceiver *receiver) = 0;
};

// A receiver is an object that can receive pixel data
class IReceiver : public IRegisterable
{
public:
    // The sender calls this function to check whether the receiver is ready to receive data.
    virtual bool ready() = 0;

    // The sender will call this function before setData to tell you the length of the
    // data you are going to receive. Implement this function to allocate enough
    // memory. Be sure to check if the length has actually changed before
    // starting the allocation process
    virtual void setLength(int length) = 0;

    // The sender will call this function when it has new data. You have to store
    // the data in your buffer. Size is in bytes.
    virtual void setData(uint8_t *data, int size) = 0;

    // After all data is set, the sender calls show() as a sign for you to
    // send out the data. This should be done asynchronously, and this function should
    // return as fast as possible.
    virtual void show() = 0;
};

// An IIinput is an input to the entire system. It is a source where
// pixels data is generated or comes in externally
class IInput : public ISender
{
public:
    // Implement this function to read data from the external source and
    // send it to connected receiver
    virtual void process() = 0;

    virtual FPSCounter *getFpsCounter() = 0;
};

// An IOutput is an output to the entire system. It is a sink where
// pixel data leaves the system. Eg to a display, network or hardware port.
class IOutput : public IReceiver
{
public:
    virtual FPSCounter *getFpsCounter() { return nullptr; }
    virtual void clear() = 0;

    // virtual int getNumPixels()=0;
};

class IConverter : public ISender, public IReceiver
{
};