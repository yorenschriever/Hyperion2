#pragma once
#include "abstractTempo.h"
#include "log.hpp"
#include "utils.hpp"

#define DEFAULT_PERIOD 0 // default off

class ConstantTempo : public AbstractTempo
{

public:
    ConstantTempo(int bpm)
    {
        setBpm(bpm);
        sourceName = "Constant";
    }

    void setBpm(int bpm)
    {
        period = 60000000 / bpm;
        Log::info(TAG,"Setting constant tempo to %d BPM",bpm);
        //The next 2 lines should be in reverse order, but if i do that, i get a 
        //segfault on docker om my M1 machine. I dont understand why.
        validSignal = true;
        startingpoint = Utils::micros();
    }

    friend class Tempo;

private:
    int period = DEFAULT_PERIOD;
    unsigned long startingpoint = -1;
    const char *TAG = "CONSTANT_TEMPO";

    void TempoTask() override
    {
        if (!validSignal || period == 0)
            return;

        int newBeatNr = (Utils::micros() - startingpoint) / period;

        // Log::info(TAG, "ConstantTempoTask: %d", newBeatNr);

        if (newBeatNr != beatNumber)
            beat(newBeatNr, period / 1000);
    }
};
