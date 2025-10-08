#pragma once

#include "../bufferPool.hpp"
#include "../fpsCounter.hpp"
#include "../interfaces.hpp"
#include <inttypes.h>

// struct InputBuffer
// {
//     uint8_t *data;
//     int length;
// };

// Derive form this class when you are going to write new inputs. You should at least implement
// getData(), and you can implement begin(), if there is some setup to do. Putting the
// initialization code in begin() instead of the constructor makes you code easier to debug,
// because at that point the logging system is guaranteed to be initialized.
class BaseInput : public IInput
{

public:
    // Implement this function to provide data to the pipe
    // When this function is called you should put copy the
    // most recent pixel data into the dataPtr and return
    // the number of byte you have written. Ideally you should
    // have a buffer ready to be copied, as every execution in
    // this function slows down the main loop.
    virtual Buffer *getData() = 0;

    virtual void process() override
    {
        if (!receiver || !receiver->ready())
            return;

        auto inputBuffer = getData();
        if (!inputBuffer || inputBuffer->getLength() == 0)
            return;

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

protected:
    IReceiver *receiver = nullptr;
    FPSCounter fpsCounter;
};
