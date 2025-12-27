#pragma once

#include "../bufferPool.hpp"
#include "../fpsCounter.hpp"
#include "../interfaces.hpp"
#include <inttypes.h>

// Derive from this class when you are going to write new inputs. You should at least implement
// getData(), and you can implement begin(), if there is some setup to do. Putting the
// initialization code in begin() instead of the constructor makes you code easier to debug,
// because at that point the logging system is guaranteed to be initialized.
class BaseInput : public IInput
{

public:
    virtual void process() override
    {
        if (!receiver || !receiver->ready())
            return;

        auto inputBuffer = getData();
        if (!inputBuffer) return;
        if (inputBuffer->getLength() == 0){
            BufferPool::release(inputBuffer);
            return;
        }

        receiver->setLength(inputBuffer->getLength());
        receiver->setData(inputBuffer->getData(), inputBuffer->getLength());

        //TODO it would be nicer if the buffer is requested in the same method where it is released
        BufferPool::release(inputBuffer);

        receiver->show();
    }

    void setReceiver(IReceiver *receiver) override
    {
        this->receiver = receiver;
    }

    FPSCounter *getFpsCounter() override
    {
        return &fpsCounter;
    }

    void initialize() override {}

    void registerToHyperion(IRegistrant *hyp) override {
        hyp->addInput(this);
        hyp->addInitializer(this);
    }
protected:
    IReceiver *receiver = nullptr;
    FPSCounter fpsCounter;

    virtual Buffer *getData() = 0;
};
