#pragma once
// -------------------------------------------------------------------- Includes
#include <vector>
using namespace std;

#include "Palette.h"
#include "LedAnimation.h"

// -------------------------------------------------------------------- Class Definition
class ChaserLedAnimation : public LedAnimation
{
protected:
    const char *TAG = "CHASERLEDANIMATION";
    bool _forward = true;
    uint16_t _baseOffset = 0;
    // TODO build cache for all chaser properties to optimize
    //vector<CRGB*> _chaserBuffers;
    uint8_t _numChasers;
    vector<double> _speedOptions;
    uint16_t _previousBeatCount = 0;
    uint16_t _previousBeatProgress = 0;
    uint16_t _previousRelativePosition = 0;

public:
    // TODO Use this internally instead of ANIMATION_STEP_SIZE (Although we should just be making sure we use the full uint16_t range)
    const static uint16_t relativeSize = 10000;

    ChaserLedAnimation(vector<LedShape> ledShapes, double baseOffset, bool forward);

    void render(
        RGBA* buffer,
        int buffersize,
        vector<LedShape> ledShapes,

        Palette colorPalette,

        uint8_t speed,
        uint8_t amount,
        uint8_t size,
        uint8_t direction,
        uint8_t offset,
        uint8_t alpha,
        
        uint16_t beatProgress,
        uint16_t beatProgressMax,
        uint16_t beatCount,
        bool updateCache
    );
};
