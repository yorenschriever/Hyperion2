#pragma once

#include "timeline.h"

class TriggerTimeline : public Timeline
{
    bool lastPatternActive = false;

public:

    bool isActive(bool patternActive, int duration) {
        this->FrameStart();

        if (patternActive && !lastPatternActive)
            this->reset();

        lastPatternActive = patternActive;

        return this->GetTimelinePosition() < duration;
    }
}; 