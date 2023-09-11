// -------------------------------------------------------------------- Includes
#include "ChaserLedAnimation.h"
#include "platform/includes/log.hpp"
#include "log.hpp"

// TODOs
	// --- MUST:
    // --- SHOULD:
	// TODO Use baseOffset internally instead of ANIMATION_STEP_SIZE (Although we should just be making sure we use the full uint16_t range)
	// TODO add caching for variables that don't change every cycle
	// TODO fix full size rendering holes between adjacent chasers if size is max (so full) and amount is bigger than 1 (Doesn't happen with each size, possible rounding error)
	// ---- COULD:
	// TODO interpolate to new speeds, keeping beatsync in mind (directly increase speed, then drift towards beatsync)
	// TODO fix ANIMATION_STEP_SIZE, let's just use full 16 bit values so we can get rid of all modulo calculations
	// TODO render from the front of the animation so splitting doesn't seemingly remove stuff (this only helps the first one unfortunately)



// -------------------------------------------------------------------- Configuration
#define MIN_CHASER_SIZE 3
#define MIN_CHASER_PART_SIZE 42	// 1/6 but in 8 bits fraction of fastled (6ths = 42, 84, 127, 170, 212)
#define MAX_CHASER_PART_SIZE 255 //170
#define MAX_CHASER_AMOUNT 9

#define ANIMATION_STEP_SIZE 10000	// TODO turn this into 65535


// -------------------------------------------------------------------- Constructor
ChaserLedAnimation::ChaserLedAnimation(vector<LedShape> ledShapes, double baseOffset, bool forward)
{
	_baseOffset = baseOffset;
	_forward = forward;

	// Loop through all ledShapes and create a chaserbuffer for each shape
	for (unsigned i = 0; i < ledShapes.size(); ++i) {
		//_chaserBuffers.push_back(new CRGB[lerp16by8(MIN_CHASER_SIZE, ledShapes[i].totalNumLeds, MAX_CHASER_PART_SIZE)]);
	}

	// Beat synced speed options
	// 12 = It takes 12 beats for each chaser to do a full shape traversal
	_speedOptions = {48, 24, 12, 9, 8, 6, 4, 3, 2, 1};
}

// -------------------------------------------------------------------- Render
void ChaserLedAnimation::render(
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
	// Update variables & buffers
	uint8_t chaserLengthFraction8 = lerp8by8(MIN_CHASER_PART_SIZE, MAX_CHASER_PART_SIZE, size);	// Cache
	uint16_t relativeChaserDelta = ANIMATION_STEP_SIZE / _numChasers;	// Cache
	int beatSpeed = (int)_speedOptions[(unsigned)lerp8by8(0, _speedOptions.size() - 1, speed)];	// Cache
	uint16_t offsetRelativeDistance = (offset > 0) ? relativeChaserDelta * offset / 255 : 0;	// Cache

	if (updateCache) {
		// Update variables
		_numChasers = lerp8by8(1, MAX_CHASER_AMOUNT, amount);
		// Loop through all ledSets in all ledShapes and render an appropriate chaser to each size buffer
		/*
		for (unsigned i = 0; i < ledShapes.size(); ++i) {
			CRGB* chaserBuffer = _chaserBuffers[i];
			uint16_t chaserPixelSize = scale16by8(ledShapes[i].totalNumLeds, chaserLengthFraction8) / _numChasers; // Cache
			if (chaserPixelSize < MIN_CHASER_SIZE) {
				chaserPixelSize = MIN_CHASER_SIZE;
			}

			for (uint16_t j = 0; j < chaserPixelSize; ++j) {
				chaserBuffer[j] = ColorFromPalette(colorPalette, map(j, 0, chaserPixelSize, 0, 255), 255, LINEARBLEND);
			}
		}
		*/
	}

	// Update position
	uint16_t previousRelativePositionAtCurrentSpeed = ((((_previousBeatCount % beatSpeed) * beatProgressMax) + _previousBeatProgress) * ANIMATION_STEP_SIZE) / (beatProgressMax * beatSpeed);
	uint16_t relativePositionAtCurrentSpeed = ((((beatCount % beatSpeed) * beatProgressMax) + beatProgress) * ANIMATION_STEP_SIZE) / (beatProgressMax * beatSpeed);
	if (relativePositionAtCurrentSpeed < previousRelativePositionAtCurrentSpeed) {
		relativePositionAtCurrentSpeed += ANIMATION_STEP_SIZE;
	}
	uint16_t relativePositionDelta = relativePositionAtCurrentSpeed - previousRelativePositionAtCurrentSpeed;
	uint16_t relativePosition = _previousRelativePosition;
	if ((_forward && direction < 128) || (!_forward && direction >= 128)) {
		relativePosition += relativePositionDelta;
		if (relativePosition > ANIMATION_STEP_SIZE) {
			relativePosition -= ANIMATION_STEP_SIZE;
		}
	} else {
		if (relativePosition < relativePositionDelta) {
			relativePosition += ANIMATION_STEP_SIZE;
		}
		relativePosition -= relativePositionDelta;
	}
	if (beatCount < _previousBeatCount) {
		// This means a beat sync came through, reset to the absolute calculated value
		relativePosition = relativePositionAtCurrentSpeed;
	}

	// Store some values of this cycle so we can compare
	_previousBeatCount = beatCount;
	_previousBeatProgress = beatProgress;
	_previousRelativePosition = relativePosition;

	// Check whether or not we should be rendering at all
	if (alpha == 0) {
		return;
	}

	// Now actually render the chasers onto their positions
	int bufferIndex = -1;
	for (unsigned i = 0; i < ledShapes.size(); ++i) {
		LedShape ledShape = ledShapes[i];

		uint16_t chaserPixelSize = scale16by8(ledShapes[i].totalNumLeds, chaserLengthFraction8) / _numChasers; // Cache
		uint16_t relativeChaserLength = scale16by8(ANIMATION_STEP_SIZE, chaserLengthFraction8) / _numChasers;  // Cache
		if (chaserPixelSize < MIN_CHASER_SIZE) {
			chaserPixelSize = MIN_CHASER_SIZE;
			relativeChaserLength = ANIMATION_STEP_SIZE * chaserPixelSize / ledShape.totalNumLeds;
		}	// Cache
		
		uint16_t relativeDirectionOffset = 0; // Cache
		if (_forward) {
			// Make sure the relative position corresponds to the front of the chaser
			relativeDirectionOffset = ANIMATION_STEP_SIZE - relativeChaserLength;	// Cache
		}

		// Calculate offset
		uint16_t offsettedRelativePosition = (_baseOffset + relativePosition + relativeDirectionOffset + offsetRelativeDistance * i) % ANIMATION_STEP_SIZE;
		
		uint16_t p = 0;
		//yoren: i changed the way we iterate
		//for (CRGBSet & ledSet : ledShape.ledSets) {
		//	for (CRGB & pixel : ledSet) {
		//Log::info(TAG,"ledshape: %d",ledShape.numLedsPerSet);
		for (int ledSetIndex = 0 ; ledSetIndex<ledShape.numSets; ledSetIndex++) {
			for(int pixelIndex= 0; pixelIndex<ledShape.numLedsPerSet; pixelIndex++) {
				bufferIndex++;
				if (bufferIndex >= buffersize){
					Log::error(TAG,"ChaserLedAnimation tried to write outside the pixelBuffer");
					return;
				}

				uint16_t pixelPosition = ANIMATION_STEP_SIZE * p++ / ledShape.totalNumLeds;	// Cache
				uint16_t positionModulo = (ANIMATION_STEP_SIZE + pixelPosition - offsettedRelativePosition) % relativeChaserDelta;
				if (positionModulo <= relativeChaserLength) {
					uint16_t renderPosition = positionModulo * ledShape.totalNumLeds / ANIMATION_STEP_SIZE;
					uint16_t bufferPosition = renderPosition;
					int deltaBuffer = 1;
					if (!_forward) {
						bufferPosition = chaserPixelSize - bufferPosition - 1;
						deltaBuffer = -1;
					}

					// TODO Optimize the interpolationRatio calculation!
					uint8_t interpolationRatio = ((positionModulo * ledShape.totalNumLeds) - (((positionModulo * ledShape.totalNumLeds) / ANIMATION_STEP_SIZE) * ANIMATION_STEP_SIZE)) * 255 / ANIMATION_STEP_SIZE;
					
					//yoren: changed datatypes, added pointer to pixel
					RGBA color = ColorFromPalette(colorPalette, map(bufferPosition, 0, chaserPixelSize, 0, 255), 255, LINEARBLEND);
					RGBA color2 = ColorFromPalette(colorPalette, map(bufferPosition + deltaBuffer, 0, chaserPixelSize, 0, 255), 255, LINEARBLEND);
					RGBA color3 = ColorFromPalette(colorPalette, map(bufferPosition + ((chaserPixelSize - 1) * - deltaBuffer), 0, chaserPixelSize, 0, 255), 255, LINEARBLEND);
					auto pixel = &buffer[bufferIndex];

					if (renderPosition == 0) {
						if (size < 255) {
							*pixel = blend(*pixel, color, scale8(interpolationRatio, alpha));
						} else {
							// Special case where the chasers link up, so we need to blend the start pixel into the next one
							*pixel = blend(*pixel, blend(color, color2, interpolationRatio), alpha);
						}
					} else if (renderPosition < chaserPixelSize - 1) {
						*pixel = blend(*pixel, blend(color, color2, interpolationRatio), alpha);
					} else if (renderPosition == chaserPixelSize - 1) {
						if (size < 255) {
							*pixel = blend(color, *pixel, 255 - alpha + scale8(interpolationRatio, alpha));
						} else {
							// Special case where the chasers link up, so we only need to blend this according to "normal" rules (since we already blended the start into the end)
							*pixel = blend(*pixel, blend(color, color3, interpolationRatio), alpha);
						}
					}
				}
			}
		}
	}
}

