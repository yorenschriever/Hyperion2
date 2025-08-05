#pragma once

#include "tempo/tempo.h"

enum WatcherType
{
    Rising,
    Falling,
    Changing
};

template <class T>
class Watcher
{

    using WatcherFunc = T (*)();

public:
    Watcher(WatcherFunc getValue, WatcherType type)
    {
        this->type = type;
        this->getValue = getValue;
    }

    bool Triggered()
    {
        T value = getValue();
        bool result = false;
        if ((type == Rising || type == Changing) && value > lastValue)
            result = true;
        if ((type == Falling || type == Changing) && value < lastValue)
            result = true;
        lastValue = value;
        return result;
    }

private:
    T lastValue;
    WatcherFunc getValue;
    WatcherType type;
};

class BeatWatcher : public Watcher<int>
{
public: 
    BeatWatcher() : 
        Watcher(
            Tempo::GetBeatNumber ,
            Changing
        )
    {}

    bool Triggered(int div=0) 
    {
        if (div == 0)
            return Watcher::Triggered();
        
        // Check if the current beat number is a multiple of div
        return Watcher::Triggered() && Tempo::GetBeatNumber() % div == 0;
    }

};