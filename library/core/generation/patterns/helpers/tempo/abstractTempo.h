#pragma once

#include "utils.hpp"
#include "tempo.h"

class Tempo;

class AbstractTempo
{
public:
    int GetBeatNumber() { return beatNumber; }

    //time in ms
    int TimeBetweenBeats() { return timeBetweenBeats; }

    //time in ms
    int TimeOfLastBeat() { return timeOfLastBeat; }

    bool HasSignal() { return validSignal && Utils::millis() - timeOfLastBeat < 2000; }

    const char* Name() { return sourceName; }

    friend class Tempo;
protected:
    int beatNumber=-1;
    int timeBetweenBeats;
    unsigned long timeOfLastBeat;
    unsigned long timeOfSecondToLastBeat;
    const char * sourceName="";
    bool validSignal=false;

    virtual void TempoTask() {}

    void beat()
    {
        beat(beatNumber+1);
    }

    void beat(int beatNumber, int timeBetweenBeats=0, bool broadcast=true)
    {
        this->beatNumber = beatNumber;
        this->timeOfSecondToLastBeat = this->timeOfLastBeat;
        this->timeOfLastBeat = Utils::millis();
        this->timeBetweenBeats = timeBetweenBeats ? timeBetweenBeats : this->timeOfLastBeat - this->timeOfSecondToLastBeat;

        if (broadcast)
            Tempo::BroadcastBeat(this);
    }
};
