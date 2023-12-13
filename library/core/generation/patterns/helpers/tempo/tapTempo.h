#pragma once
#include "abstractTempo.h"
#include "utils.hpp"

//#define DEFAULT_PERIOD 480000 //default at 125BPM
#define DEFAULT_PERIOD 0 // default off

class TapTempo : public AbstractTempo
{
private:
    static TapTempo * instance;
public:
    TapTempo(){
        sourceName="Tap";
    };

    static TapTempo * getInstance()
    {
        if (!instance)
            instance = new TapTempo();
        return instance;
    }

    void Stop()
    {
        validSignal = false;
        beatNumber = -1;
        period = 0;
    }

    // this aligns the tempo to your tap without resetting the beat count or interval
    void Align()
    {
        if (period == 0)
            return;

        long offset = (Utils::micros() - startingpoint) % period;
        if (offset > period / 2)
            offset -= period; // syncing to the other side is closer
        startingpoint += offset;
    }

    //
    // void BarAlign()
    // {
    //     unsigned long timepassed = micros()-startingpoint;
    //     unsigned long timeinbar = timepassed % (period*4);
    //     startingpoint = micros()-timeinbar; //micros() - timesincebarstart;
    // }

    void Tap()
    {
        unsigned long now = Utils::micros();

        if (!isTapping())
        {
            // start a new tap session
            firstTap = now;
            startingpoint = now; // for now the Tempo counter starting point is linked to the first tap
            tapCount = 0;
            validSignal = true;
            beat(0, period / 1000); // this wont be visible, because if there is no signal, the beatnumber will be 0, so setting it to 0 again doesnt trigger the watcher
        }

        tapCount++;
        lastTap = now;

        if (tapCount > 1)
        {
            period = (lastTap - firstTap) / (tapCount - 1);
            if (period < 1)
                period = DEFAULT_PERIOD;
            validSignal = true;
        }

        timeBetweenBeats = period / 1000;
    }

private:
    const int tapTimeout = 1000000; // if no tap is detected for tapTimeout us, the next tap will be interpreted as a new tap session
    int period = DEFAULT_PERIOD;     
    unsigned long startingpoint;
    unsigned long lastTap;
    unsigned long firstTap;
    int tapCount;

    bool isTapping()
    {
        return (Utils::micros() - lastTap) < tapTimeout;
    }

    void TempoTask() override
    {
        if (!validSignal || period == 0)
            return;

        int newBeatNr = (Utils::micros() - startingpoint) / period;
        if (newBeatNr != beatNumber)
            beat(newBeatNr, period / 1000);
    }
};


TapTempo * TapTempo::instance = nullptr;