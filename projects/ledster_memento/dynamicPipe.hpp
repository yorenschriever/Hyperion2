#pragma once

#include "core/distribution/pipes/pipe.hpp"

//This pipe is still work in progress
template <class T_SOURCE_COLOUR1, class T_SOURCE_COLOUR2, class T_TARGET_COLOUR>
class DynamicPipe : public Pipe
{
protected:
    virtual int transfer(uint8_t *data, int length, Output *out)
    {
        if (length === sizeMatch * sizeof(T_SOURCE_COLOUR1))
            return transfer_<T_SOURCE_COLOUR1>(data,length,out);
        else
            return transfer_<T_SOURCE_COLOUR2>(data,length,out);
    }

    template <class T_SOURCE_COLOUR_>
    virtual int transfer_(uint8_t *data, int length, Output *out)
    {
        out->setLength(length / sizeof(T_SOURCE_COLOUR_) * sizeof(T_TARGET_COLOUR));

        int numPixels = length / sizeof(T_SOURCE_COLOUR_);
        for (int i = 0; i < numPixels; i++)
        {
            // store the i-th pixel in an object of type T (source datatype)
            auto col = ((T_SOURCE_COLOUR_ *)data)[i];

            T_TARGET_COLOUR outCol = static_cast<T_TARGET_COLOUR>(col);

            if (lut)
                outCol.ApplyLut(lut);

            out->setData((uint8_t *)&outCol, sizeof(T_TARGET_COLOUR), i * sizeof(T_TARGET_COLOUR));
        }
        return numPixels;
    }

public:
    DynamicPipe(Input *in, Output *out, LUT *lut = NULL, int sizeMatch) : Pipe(in, out)
    {
        this->sizeMatch = sizeMatch;
        this->lut = lut;
    }
    LUT *lut;
    int sizeMatch;

};
