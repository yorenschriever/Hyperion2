#pragma once

#include "utils.hpp"

// these are easier to type imo
#define FadeDown Fade<Down>
#define FadeUp Fade<Up>

enum FadeEase
{
    Linear,
    Quadratic,
    Cubic
};

enum FadeDirection
{
    Up,
    Down
};

enum FadeWaitPosition
{
    WaitAtStart = 0,
    WaitAtEnd = 1
};

template <FadeDirection DIRECTION = Down, FadeEase EASE = Linear>
class Fade
{

private:
    unsigned long startingpoint = 0;

    // FadeEase ease;
    // FadeDirection direction;
    FadeWaitPosition waitPosition = WaitAtStart;

public:
    /***
     * Creates a fade
     * duration = duration of the fade in ms
     * waitPosition: when using startDelay(), should the fade wait at start (phase=0) or at the end (phase=1)
     */
    Fade(
        unsigned int duration = 1000,
        FadeWaitPosition waitPosition = WaitAtEnd)
    {
        this->duration = duration;
        this->waitPosition = waitPosition;
        reset();
    }

    

    float getPhase() { return getPhase(0, duration); }
    float getPhase(int startDelay) { return getPhase(startDelay, duration); }
    float getPhase(int startDelay, int durationArg)
    {
        unsigned long now = Utils::millis();

#if ESP_PLATFORM
        // building with esp-idf gives me a compiler error:
        // internal compiler error: in extract_constrain_insn, at recog.cc:2692
        // Waiting for a fix will probably takes ages, because it needs to
        // be fixed in gcc, and then esp-idf needs to bump to the new version.
        // I came up with this workaround.
        // I narrowed this error down to using the millis() function here.
        // strangely, in LFO (which is very similar) it is working.
        // Here i do a modulus operator to it, so i tried it here as well.
        // The second operand of the operator is the biggest value
        // of unsigned long, so this is basically a no-op
        now = now % ((unsigned long)-1);
#endif

        int phase = (now - startingpoint - startDelay);

        if (phase < 0)
            return waitPosition;

        float result = ((float)phase / (float)durationArg);
        if (result > 1)
            return 1;

        return result;
    }

    // value between 0-1
    float getValue() { return getValue(0, duration); }
    float getValue(int startDelay) { return getValue(startDelay, duration); }
    float getValue(int startDelay, int durationArg)
    {
        float phase = getPhase(startDelay, durationArg);

        if (DIRECTION == Down)
            phase = 1 - phase;

        switch (EASE)
        {
        case (Linear):
            return phase;
            break;
        case (Quadratic):
            return pow(phase, 2);
            break;
        case (Cubic):
            return pow(phase, 4);
            break;
        default:
            return phase;
        }
    }

    void reset()
    {
        this->startingpoint = Utils::millis();
    }

    bool isFinished(int maxDelay = 0)
    {
        return Utils::millis() - startingpoint > duration + maxDelay;
    }

    void setDuration(int duration) {
        this->duration = duration;
    }

    // This is a helper function to set the duration when using fade to create spatial effects.
    // length = the length of the tail, measured relative to the size of your chase.
    // maxDelay = the max value that you are going to pass to getValue(startDelay);
    void setTailLength(float length, int maxDelay) {
        setDuration(length * maxDelay);
    }

//private:
    unsigned int duration = 1000;
};