#pragma once

class FPSCounter
{
private:
    int volatile usedFrameCount = 0;
    int volatile missedFrameCount = 0;

public:

    void increaseUsedFrameCount(int amount=1) { usedFrameCount+=amount; }
    void increaseMissedFrameCount(int amount=1) { missedFrameCount+=amount; }

    int getUsedFrameCount() { return usedFrameCount; }
    int getMissedFrameCount() { return missedFrameCount; }
    int getTotalFrameCount() { return missedFrameCount + usedFrameCount; }
    void resetFrameCount()
    {
        usedFrameCount = 0;
        missedFrameCount = 0;
    }
};