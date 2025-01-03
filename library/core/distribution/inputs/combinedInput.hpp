#pragma once
#include "input.hpp"
#include <vector>

using ConvertFunc = int (*)(uint8_t *src, int length, uint8_t *dst, int dst_length, LUT *lut);

struct CombinedInputPart
{
    Input *input;
    int offset;
    ConvertFunc convert = nullptr;
    LUT *lut = nullptr;
};

// CombinedInput reads the input from multiple sources and glues them together
class CombinedInput : public Input
{

public:
    CombinedInput(std::vector<CombinedInputPart> parts, int bufferSize)
    {
        this->parts = parts;
        this->bufferSize = bufferSize;
    }

    virtual void begin()
    {
        dataBuffer = (uint8_t *)malloc(bufferSize);
        // TODO only if convert is used
        convertBuffer = (uint8_t *)malloc(bufferSize);

        if (!dataBuffer)
        {
            Log::error(TAG, "Unable to allocate memory for CombinedInput, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }

        for (auto part : parts)
            part.input->begin();
    }

    virtual int loadData(uint8_t *dataPtr, unsigned int buffersize)
    {
        int totalCopied = 0;

        for (auto part : parts)
        {
            if (part.convert)
            {
                int sourceLen = part.input->loadData(convertBuffer, bufferSize);
                totalCopied += part.convert(convertBuffer, sourceLen, dataBuffer + part.offset, buffersize - part.offset, part.lut);
            }
            else
            {
                totalCopied += part.input->loadData(dataBuffer + part.offset, buffersize - part.offset);
            }
        }

        if (totalCopied == 0)
            return 0;

        usedframecount++;
        memcpy(dataPtr, dataBuffer, bufferSize);
        return bufferSize;
    }

private:
    std::vector<CombinedInputPart> parts;
    int bufferSize;
    uint8_t *dataBuffer;
    uint8_t *convertBuffer;
    const char *TAG = "CombinedInput";

public:
    template <class T_SOURCE_COLOUR, class T_TARGET_COLOUR>
    static int convert(uint8_t *src, int length, uint8_t *dst, int dst_length, LUT *lut)
    {
        int numPixels = length / sizeof(T_SOURCE_COLOUR);
        if (numPixels * sizeof(T_TARGET_COLOUR) > dst_length)
        {
            Log::error("CombinedInput", "CombineInput destination buffer too small, need %d bytes, got %d. Not all pixels are copied", numPixels * sizeof(T_TARGET_COLOUR), dst_length);
            numPixels = dst_length / sizeof(T_TARGET_COLOUR);
        }

        for (int i = 0; i < numPixels; i++)
        {
            // store the i-th pixel in an object of type T_SOURCE_COLOUR
            T_SOURCE_COLOUR col = ((T_SOURCE_COLOUR *)src)[i];

            // this is where the actual conversion takes place. static cast will use
            // the cast operators in the Colour classes to convert between the
            // representations.
            T_TARGET_COLOUR outCol = static_cast<T_TARGET_COLOUR>(col);

            // Apply lookup table to do gamma correction and handle other non-linearity
            if (lut)
                outCol.ApplyLut(lut);

            // Place the data in the output buffer
            ((T_TARGET_COLOUR *)dst)[i] = outCol;
        }
        return numPixels * sizeof(T_TARGET_COLOUR);
    }
};