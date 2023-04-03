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
        startingpoint = Utils::micros();
        validSignal = true;
    }

    friend class Tempo;

private:
    int period = DEFAULT_PERIOD;
    unsigned long startingpoint;
    const char *TAG = "CONSTANT_TEMPO";

    void TempoTask() override
    {
        Log::info(TAG, "constant tempo task");

        if (!validSignal || period == 0)
            return;

        int newBeatNr = (Utils::micros() - startingpoint) / period;

        Log::info(TAG, "ConstantTempoTask: %d", newBeatNr);

        if (newBeatNr != beatNumber)
            beat(newBeatNr, period / 1000, false); // use the beat locally, but don't broadcast it.
    }
};
