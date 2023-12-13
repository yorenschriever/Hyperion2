#pragma once
#include "abstractTempo.h"
#include "log.hpp"
#include "midiListener.hpp"

#define MIDI_CLOCK_START 0xFA
#define MIDI_CLOCK_STOP 0xFC
#define MIDI_CLOCK_CONTINUE 0xFB
#define MIDI_CLOCK_TICK 0xF8

class MidiClockTempo : public AbstractTempo, public IMidiListener
{

public:
    MidiClockTempo()
    {
        sourceName = "Midi clock";
        //Log::info(TAG, "midi clock tempo created");
    }

    void onNoteOn(uint8_t channel, uint8_t note, uint8_t value) override{};
    void onNoteOff(uint8_t channel, uint8_t note, uint8_t value) override{};
    void onControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override{};
    void onSystemRealtime(uint8_t msg) override
    {
        if (msg == MIDI_CLOCK_CONTINUE)
        {
            //stopped = false;
            validSignal = true;
            tickCount = 0;
            // beat(0);
            Log::info(TAG, "continue. aligned beat");
        }
        else if (msg == MIDI_CLOCK_START)
        {
            //stopped = false;
            validSignal = true;
            tickCount = 0;
            beat(0);
            Log::info(TAG, "start. reset beat to 0");
        }
        else if (msg == MIDI_CLOCK_STOP)
        {
            //if (stopped)
            //{
                beat(-1); // reset the beat number, so we dont get an extra flash when the signal valid timeout resets the beat to 0
                validSignal = false;
            //    Log::info(TAG, "stopped 2x. stopped signal");
            //}
            tickCount = 0;
            //stopped = true;
            //Log::info(TAG, "stop. aligned beat.");
            Log::info(TAG, "stop.");

        }
        else if (msg == MIDI_CLOCK_TICK)
        {
            if (!validSignal)
                return;
            tickCount = (tickCount + 1) % 24;
            if (tickCount == 0)
            {
                beat();
            }
        }
    }

private:
    int tickCount = 0;
    //bool stopped = true;
    const char *TAG = "MIDICLOCK";
};
