
#pragma once

#include "interfaces.hpp"
#include "bufferPool.hpp"

// ConvertPipe<SourceColour,TargetColour> will convert the data from source colour format
// to target format. eg. from RGB to RGBW or from RGB to Monochrome.
// ConvertPipe can also apply a look up table (LUT) to adjust the colour
template <class T_SOURCE_COLOUR, class T_TARGET_COLOUR>
class ConvertColor: public IConverter
{
private:
    IReceiver *receiver = nullptr;
    LUT *lut = nullptr;

public: 
    ConvertColor(LUT *lut = nullptr)
    {
        this->lut = lut;
    }

    IConverter *setReceiver(IReceiver *receiver) override
    {
        this->receiver = receiver;
        return this;
    }

    bool ready() override
    {
        return receiver && receiver->ready();   
    }

    // void begin() override
    // {
    //     receiver->begin();
    // }

    void show() override
    {
        receiver->show();
    }

    void setLength(int length) override
    {
        // tell the output the new length so it can allocate enough space for the data
        // we are going to send
        receiver->setLength(length / sizeof(T_SOURCE_COLOUR) * sizeof(T_TARGET_COLOUR));
    }

    void setData(uint8_t *data, int length) override
    {
        int numPixels = length / sizeof(T_SOURCE_COLOUR);

        int outputLength = numPixels * sizeof(T_TARGET_COLOUR);
        auto outputBuffer = BufferPool::getBuffer(outputLength);
        if (!outputBuffer)
        {
            Log::error("CONVERT_COLOR", "Unable to allocate memory for ConvertColor, free heap = %d\n", Utils::get_free_heap());
            Utils::exit();
        }
        uint8_t *outputBufferPtr = outputBuffer->getData();

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
            // receiver->setData((uint8_t *)&outCol, sizeof(T_TARGET_COLOUR), i * sizeof(T_TARGET_COLOUR));
            ((T_TARGET_COLOUR *)outputBufferPtr)[i] = outCol;
        }
        receiver->setData(outputBufferPtr, outputLength);
        BufferPool::release(outputBuffer);
    }
    
};
