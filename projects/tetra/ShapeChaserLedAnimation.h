#pragma once
// -------------------------------------------------------------------- Includes
#include <vector>
using namespace std;

#include "LedAnimation.h"

// -------------------------------------------------------------------- Class Definition
class ShapeChaserLedAnimation : public LedAnimation
{
protected:
    bool _forward = true;
    
    uint8_t _numChasers;
    vector<double> _speedOptions;

    uint16_t _relativeChaserDelta;
    uint8_t _beatSpeed;
    uint16_t _offsetRelativeDistance;
    uint16_t _relativeChaserLength;
    uint16_t _relativeDirectionOffset = 0;

    uint16_t _previousBeatCount = 0;
    uint16_t _previousBeatProgress = 0;
    uint16_t _previousRelativePosition = 0;

public:
    ShapeChaserLedAnimation(bool forward);
    
    void render(
        RGBA* buffer,
		int buffersize,
        vector<LedShape> ledShapes,

        Palette *colorPalette,

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
