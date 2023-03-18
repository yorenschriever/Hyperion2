#pragma once
#include <stdint.h>

class IMidiListener
{
public:
    virtual void onNoteOn(uint8_t channel, uint8_t note, uint8_t value) = 0;
    virtual void onNoteOff(uint8_t channel, uint8_t note, uint8_t value) = 0;
    virtual void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) = 0;
    virtual void onSystemRealtime(uint8_t message) = 0;
};