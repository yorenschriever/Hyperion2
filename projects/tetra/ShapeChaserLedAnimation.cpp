// -------------------------------------------------------------------- Includes
#include "ShapeChaserLedAnimation.h"

// -------------------------------------------------------------------- Configuration
#define SHAPE_MIN_SIZE 20		// 1/12 but in 8 bits fraction of fastled
#define SHAPE_MAX_SIZE 255
#define SHAPE_MAX_CHASER_AMOUNT 3

#define SHAPE_ANIMATION_STEP_SIZE 65535

// -------------------------------------------------------------------- Constructor
ShapeChaserLedAnimation::ShapeChaserLedAnimation(vector<LedShape> ledShapes, bool forward) : _forward(forward)
{
	// Beat synced speed options
	// 12 = It takes 12 beats for each chaser to do a full object traversal
	_speedOptions = {24, 12, 6, 4, 3, 2, 1};

	// TODO Make sure we can use a fractioned chaser amount so we can have 1 chaser going down the entire object once every x beats
	//		TODO then also build that into the horizontal chasers?
}

// -------------------------------------------------------------------- Render
void ShapeChaserLedAnimation::render(
		RGBA* buffer,
		int buffersize,
        vector<LedShape> ledShapes,

        Gradient *colorPalette,

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
    )
{
	// Update cache
	if (updateCache) {
		// Update variables
		_numChasers = lerp8by8(1, SHAPE_MAX_CHASER_AMOUNT, amount);
		_relativeChaserDelta = SHAPE_ANIMATION_STEP_SIZE / _numChasers;
		_relativeChaserLength = scale16by8(_relativeChaserDelta, map8(size, SHAPE_MIN_SIZE, SHAPE_MAX_SIZE));
		// TODO make sure we can use fractional beatspeeds for higher speeds! :)
		_beatSpeed = (int)_speedOptions[(unsigned)lerp8by8(0, _speedOptions.size() - 1, speed)];
		_offsetRelativeDistance = (offset > 0) ? 255 - (_relativeChaserDelta * offset / 255) : 0;
		_relativeDirectionOffset = - (SHAPE_ANIMATION_STEP_SIZE * (ledShapes.size() - 1)) / ledShapes.size();
		if (_forward) {
			// Make sure the relative position corresponds to the front of the chaser
			_relativeDirectionOffset = - _relativeChaserLength;
		}
	}

	
	// Update position
	uint16_t relativePositionAtCurrentSpeed = ((((beatCount % _beatSpeed) * beatProgressMax) + beatProgress) * SHAPE_ANIMATION_STEP_SIZE) / (beatProgressMax * _beatSpeed);
	uint16_t relativePosition = _previousRelativePosition;
	if (beatCount < _previousBeatCount) {
		// This means a beat sync came through, reset to the absolute calculated value
		relativePosition = relativePositionAtCurrentSpeed;
	} else {
		uint16_t previousRelativePositionAtCurrentSpeed = ((((_previousBeatCount % _beatSpeed) * beatProgressMax) + _previousBeatProgress) * SHAPE_ANIMATION_STEP_SIZE) / (beatProgressMax * _beatSpeed);
		uint16_t relativePositionDelta = relativePositionAtCurrentSpeed - previousRelativePositionAtCurrentSpeed;
		// TODO beat drift correction
		// int relativePositionBeatDriftCorrection = (previousRelativePositionAtCurrentSpeed - _previousRelativePosition) * 0.001;
		// Serial.printf("relativePositionAtCurrentSpeed %d _previousRelativePosition %d relativePositionBeatDriftCorrection %d \n", relativePositionAtCurrentSpeed, _previousRelativePosition, relativePositionBeatDriftCorrection);
		if ((_forward && direction < 128) || (!_forward && direction >= 128)) {
			// relativePosition += relativePositionDelta + relativePositionBeatDriftCorrection;
			relativePosition += relativePositionDelta;
		} else {
			// relativePosition -= relativePositionDelta - relativePositionBeatDriftCorrection;
			relativePosition -= relativePositionDelta;
		}
	}

	// Store some values of this cycle so we can compare
	_previousBeatCount = beatCount;
	_previousBeatProgress = beatProgress;
	_previousRelativePosition = relativePosition;


	// Check whether or not we should be rendering at all
	if (alpha == 0) {
		return;
	}

	RGBA* pixel=buffer;
	// Actual rendering
	for (unsigned i = 0; i < ledShapes.size(); ++i) {
		LedShape ledShape = ledShapes[i];
		// Calculate relative shape position
		uint16_t shapePosition = SHAPE_ANIMATION_STEP_SIZE * i / ledShapes.size();	// Cache
		for (unsigned j = 0; j< ledShape.numSets; ++j) {
			//CRGBSet ledSet = ledShape.ledSets[j];
			// Calculate offset
			uint16_t offsettedRelativePosition = relativePosition + _relativeDirectionOffset + _offsetRelativeDistance * j;

			// Calculate modulo // TODO find more descriptive name
			uint16_t positionModulo = (SHAPE_ANIMATION_STEP_SIZE + shapePosition - offsettedRelativePosition) % _relativeChaserDelta;
			if (positionModulo <= _relativeChaserLength) {
				uint8_t colorPos = map(positionModulo, 0, _relativeChaserLength, 0, 255);
				if (!_forward) {
					colorPos = 255 - colorPos;
				}
				for (int k=0; k< ledShape.numLedsPerSet; k++) {
					//*pixel = blend(pixel, ColorFromPalette(colorPalette, colorPos, 255), alpha);
					*pixel = ColorFromPalette(colorPalette, colorPos, 255);
					pixel++;
				}
			} else {
				pixel+=ledShape.numLedsPerSet;
			}
		}
	}
}

