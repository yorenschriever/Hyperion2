#pragma once

#include "core/generation/controlHub/controlHub.hpp"
#include "core/generation/patterns/pattern.hpp"
#include "input.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <algorithm>
#include <vector>

// this class attaches a provided set of patterns to a control hub.
// Patterns should all be the same 'Colour'. You have to provide this colour as template here
template <class T_COLOUR>
class ControlHubInput : public Input
{
public:
    typedef struct
    {
        int column;
        int slot;
        Pattern<T_COLOUR> *pattern;
    } SlotPattern;

    // length = the length in pixels
    // hub = the controlHub to listen to
    // slotPatterns = a list of patterns and the position in the control hub that they should be attached to
    // This constructor give you the most freedom. You can attach slots in multiple columns to this input, 
    // which means that you not bound to output type when assigning columns in the control hub.
    // Also you can attach multiple patches to the same slot (also from multiple ControlHubInputs), 
    // so you can create scenes with for multiple outputs under 1 button
    ControlHubInput(int length, ControlHub *hub, std::vector<SlotPattern> slotPatterns)
    {
        this->length = length;
        this->slotPatterns = slotPatterns;
        this->hub = hub;
        this->ledData = (T_COLOUR *)malloc(length * sizeof(T_COLOUR));

        if (!this->ledData)
        {
            Log::error("CONTROL_HUB_INPUT", "Unable to allocate memory for ControlHubInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
    }

    //simplified version of the constructor, where all patterns are added to a single column in the control hub
    ControlHubInput(int length, ControlHub *hub, int column, std::vector<Pattern<T_COLOUR> *>patterns)
    {
        std::vector<SlotPattern> slotPatterns;
        int slotIndex=0;
        for (auto pattern: patterns){
            slotPatterns.push_back({.column = column, .slot = slotIndex++, .pattern = pattern});
        }
        ControlHubInput(length, hub, slotPatterns);
    }

    void begin() override
    {
        for (int i = 0; i < length; i++)
            this->ledData[i] = T_COLOUR();

        for (auto slotPattern : slotPatterns)
            slotPattern.pattern->Initialize();
    }

    int loadData(uint8_t *dataPtr, unsigned int buffersize) override
    {
        int safeLength = std::min(length, (int)(buffersize / sizeof(T_COLOUR)));
        if (safeLength != length)
        {
            Log::error("CONTROL_HUB_INPUT", "loadData buffer size not large enough");
        }

        for (int i = 0; i < safeLength; i++)
            ((T_COLOUR *)dataPtr)[i] = T_COLOUR();

        for (auto slotPattern : slotPatterns)
        {
            for (int i = 0; i < safeLength; i++)
                ledData[i] = T_COLOUR();

            auto column = hub->findColumn(slotPattern.column);
            auto slot = hub->findSlot(column, slotPattern.slot);
            uint8_t dimValue = (int) column->dim * hub->masterDim >> 8;

            //Log::info("CONTROL_HUB_INPUT", "calculating pattern active = %d, dim = %d ,masterDim = %d", slot->activated, column->dim, hub->masterDim);

            slotPattern.pattern->Calculate(ledData, safeLength, slot->activated);

            // apply dimming and copy to output buffer
            for (int i = 0; i < safeLength; i++)
            {
                // TODO give a good thought about when we want dimming and when we want to make things transparent.
                // how to support this in general?
                if (std::is_same<T_COLOUR, RGBA>::value)
                {
                    RGBA *l = &ledData[i];
                    l->A = (l->A * dimValue) >> 8;
                }
                else
                {
                    ledData[i].dim(dimValue);
                }
                ((T_COLOUR *)dataPtr)[i] += ledData[i];
            }
        }

        usedframecount++;
        return safeLength * sizeof(T_COLOUR);
    }

private:
    int length = 0;
    T_COLOUR *ledData;

    std::vector<SlotPattern> slotPatterns;
    ControlHub *hub;
};