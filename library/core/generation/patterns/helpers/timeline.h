
#pragma once

#include "utils.hpp"
#include <stdint.h>

struct TimelineEvent
{
    int Moment;
};

class Timeline
{

public:
    Timeline(int repeatAfter = 0)
    {
        this->duration = repeatAfter;
        reset();
    }

    bool Happened(TimelineEvent event)
    {
        return Happened(event.Moment);
    }

    // Returns true if the moment happened within the frame.
    // So this is just a single frame spike
    // use GetTimelinePosition() < moment for a level that stays on
    bool Happened(int moment)
    {
        if (startInterval == endInterval)
            return false;

        if (startInterval < endInterval)
            return moment >= startInterval && moment < endInterval;

        //case (startInterval > endInterval)
        //used in case the interval looped back to the start
        return moment >= startInterval || moment < endInterval;
    }

    void FrameStart()
    {
        startInterval = endInterval;
        endInterval = GetTimelinePosition();
    }

    //get the position on the timeline in ms.
    //if duration > 0 it loops back so that the result always fall between 0 and duration
    unsigned int GetTimelinePosition()
    {
        unsigned int result = Utils::millis() - startingpoint;
        if (duration > 0)
            result = result % duration;
        return result;
    }

    void SetDuration(int duration)
    {
        if (duration == this->duration)
            return;
        //correct the phase so the period change doesn't result in a phase change
        float phase = GetTimelinePosition();
        this->startingpoint = Utils::millis() - phase * duration;
        this->duration = duration;
    }

    void reset()
    {
        this->startingpoint = Utils::millis();
    }

protected:
    unsigned long startingpoint=0;
    int duration=1000;
    unsigned int startInterval =0;
    unsigned int endInterval = 0;
};
