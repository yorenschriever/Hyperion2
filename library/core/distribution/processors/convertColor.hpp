
#pragma once

#include "../interfaces.hpp"

// ConvertColor<SourceColor,TargetColor> will convert the data from source color format
// to target format. eg. from RGB to RGBW or from RGB to Monochrome.
// ConvertColor can also apply a look up table (LUT) to adjust the color
template <class T_SOURCE_COLOR, class T_TARGET_COLOR>
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

    Buffer process(const Buffer& inputBuffer) override
    {
        int numPixels = inputBuffer.size() / sizeof(T_SOURCE_COLOR);
        auto outputBuffer = Buffer(numPixels * sizeof(T_TARGET_COLOR));
        
        // Log::info("ConvertColor", "Converting %d pixels from %d to %d", numPixels, sizeof(T_SOURCE_COLOR), sizeof(T_TARGET_COLOR));

        for (int i = 0; i < numPixels; i++)
        {
            // store the i-th pixel in an object of type T_SOURCE_COLOR
            T_SOURCE_COLOR col = inputBuffer.as<T_SOURCE_COLOR>()[i];

            // this is where the actual conversion takes place. static cast will use
            // the cast operators in the Color classes to convert between the
            // representations.
            T_TARGET_COLOR outCol = static_cast<T_TARGET_COLOR>(col);

            // Apply lookup table to do gamma correction and handle other non-linearity
            if (lut)
                outCol.ApplyLut(lut);

            // Pass the data to the output
            outputBuffer.as<T_TARGET_COLOR>()[i] = outCol;
        }
        return outputBuffer;
    }
    
};
