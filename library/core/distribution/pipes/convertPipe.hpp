#pragma once

#include "pipe.hpp"
// ConvertPipe<SourceColour,TargetColour> will convert the data from source colour format
// to target format. eg. from RGB to RGBW or from RGB to Monochrome.
// ConvertPipe can also apply a look up table (LUT) to adjust the colour
template <class T_SOURCE_COLOUR, class T_TARGET_COLOUR>
class ConvertPipe:
{
protected:
    // Transfer function that can convert between colour representations and apply LUTs
    // This function has been thoroughly optimized, because this is the workhorse of the
    // device. When making changes be absolutely sure that you keep performance in mind.
    // See docs/speed and call stack for steps taken to optimize
    virtual int transfer(uint8_t *data, int length, Output *out)
    {
        // tell the output the new length so it can allocate enough space for the data
        // we are going to send
        out->SetLength(length / sizeof(T_SOURCE_COLOUR) * sizeof(T_TARGET_COLOUR));

        int numPixels = length / sizeof(T_SOURCE_COLOUR);
        for (int i = 0; i < numPixels; i++)
        {
            // store the i-th pixel in an object of type T (source datatype)
            T_SOURCE_COLOUR col = ((T_SOURCE_COLOUR *)data)[i];

            // this is where the actual conversion takes place. static cast will use
            // the cast operators in the Colour classes to convert between the
            // representations.
            T_TARGET_COLOUR outCol = static_cast<T_TARGET_COLOUR>(col);

            // Apply lookup table to do gamma correction and handle other non-linearity
            if (lut)
                outCol.ApplyLut(lut);

            // Pass the data to the output
            // TODO why provide the data pixel by pixel als have a function call overhead for each
            // pixel. cant we dump the entire byte array in one go?
            // What about a function that passes us a data pointer, and we place the items directly into it
            // this saves a function call for each pixel, and an additional memcpy.
            out->SetData((uint8_t *)&outCol, sizeof(T_TARGET_COLOUR), i * sizeof(T_TARGET_COLOUR));
        }
        return numPixels;
    }

public:
    ConvertPipe(Input *in, Output *out, LUT *lut = NULL) : Pipe(in, out)
    {
        this->lut = lut;
    }
    LUT *lut;
};
