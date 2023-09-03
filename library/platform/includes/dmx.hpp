#pragma once

#include <stdint.h>

class DMX
{
public:
    static DMX *getInstance(unsigned int port);

    virtual uint8_t read(uint16_t channel) = 0; // returns the dmx value for the given address (values from 1 to 512)
    virtual bool isHealthy() = 0;               // returns true when a valid DMX signal has been received within the last 500ms
    virtual int getFrameNumber() = 0;           // get the number of frames received
    virtual uint8_t *getDataPtr() = 0;          // get a pointer to the incoming data buffer

    virtual void write(uint8_t *data, int len, int startChannel) = 0; // write to the dmx output buffer. After all data is written, use show() to send out the frame. startChannel >= 1
    virtual void show() = 0;                                   // sends the dmx frame
    virtual bool txBusy() = 0;
    virtual void clearTxBuffer() = 0;

    virtual void sendFullFrame(bool) = 0;                                // if true: always send all 512 channels. if false: shrink the universe to the number of used channels
    virtual void setUniverseSize(int minsize, int trailingchannels) = 0; // if not sending full frame you can set a minimum frame size and number of trailing channels here.

    virtual ~DMX() = default;

protected:
    DMX(){}
};