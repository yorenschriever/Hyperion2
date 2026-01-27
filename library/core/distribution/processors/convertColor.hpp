
#pragma once

#include "../interfaces.hpp"

// ConvertColor<SourceColour,TargetColour> will convert the data from source colour format
// to target format. eg. from RGB to RGBW or from RGB to Monochrome.
// ConvertColor can also apply a look up table (LUT) to adjust the colour
template <class T_SOURCE_COLOUR, class T_TARGET_COLOUR>
class ConvertColor final:  public IConverter
{
private:
    LUT *lut = nullptr;

public: 
    ConvertColor(LUT *lut = nullptr)
    {
        this->lut = lut;
    }

    void initialize() override {}

    bool ready() override
    {
        return true;   
    }

    Buffer process(Buffer inputBuffer) override
    {
        int numPixels = inputBuffer.size() / sizeof(T_SOURCE_COLOUR);
        auto outputBuffer = Buffer(numPixels * sizeof(T_TARGET_COLOUR));
        
        // Log::info("ConvertColor", "Converting %d pixels from %d to %d", numPixels, sizeof(T_SOURCE_COLOUR), sizeof(T_TARGET_COLOUR));

        for (int i = 0; i < numPixels; i++)
        {
            // store the i-th pixel in an object of type T_SOURCE_COLOUR
            T_SOURCE_COLOUR col = inputBuffer.as<T_SOURCE_COLOUR>()[i];

            // this is where the actual conversion takes place. static cast will use
            // the cast operators in the Colour classes to convert between the
            // representations.
            T_TARGET_COLOUR outCol = static_cast<T_TARGET_COLOUR>(col);

            // Apply lookup table to do gamma correction and handle other non-linearity
            if (lut)
                outCol.ApplyLut(lut);

            // Pass the data to the output
            outputBuffer.as<T_TARGET_COLOUR>()[i] = outCol;
        }
        return outputBuffer;
    }
    
};
