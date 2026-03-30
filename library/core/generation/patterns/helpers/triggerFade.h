#pragma once

#include "fade.h"

template <FadeDirection DIRECTION = Down, FadeEase EASE = Linear>
class TriggerFade : public Fade<DIRECTION, EASE>
{
    bool lastPatternActive = false;
    BeatWatcher watcher;

public:
    TriggerFade(
        unsigned int duration = 1000,
        FadeWaitPosition waitPosition = WaitAtEnd) : 
        Fade<DIRECTION, EASE>(duration, waitPosition)
    {}

    // See Fade.isFinished for documentation on the arguments.
    bool isActive(bool patternActive, int maxDelay = 0, int maxDuration = -1) {
        if (patternActive && !lastPatternActive)
            this->reset();

        if (patternActive && watcher.Triggered())
            this->reset();

        lastPatternActive = patternActive;

        return !this->isFinished(maxDelay, maxDuration);
    }
}; 