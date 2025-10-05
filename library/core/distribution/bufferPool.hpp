
#pragma once
#include <cstdlib>
#include <inttypes.h>
#include <vector>

//This is written for use in the distribution module only.
//If this is going to be called from outside the distribution module, it should be rewritten to be thread safe.
class BufferPool;

class Buffer
{
friend BufferPool;
public:
    void release()
    {
        inUse = false;
    }
    uint8_t *getData()
    {
        return data;
    }
    int getLength()
    {
        return length;
    }

private:
    uint8_t *data;
    int length;
    bool inUse = false;
};

class BufferPool
{
private:
    // static uint8_t * buffer;
    // static int bufferSize;
    static std::vector<Buffer*> buffers;

public:
    static Buffer *getBuffer(int size)
    {
        // if (size > bufferSize)
        // {
        //     buffer = (uint8_t *)realloc(buffer, size);
        //     bufferSize = size;
        // }
        // return buffer;

        Buffer * largestFreeBuffer = nullptr;

        //see if a free buffer is available
        for (auto buffer : buffers)
        {
            if (buffer->inUse)
                continue;

            if (buffer->length >= size)
            {
                buffer->inUse = true;
                return buffer;
            }

            if (largestFreeBuffer == nullptr || buffer->length > largestFreeBuffer->length)
                largestFreeBuffer = buffer;
        }

        //make the largest buffer a bit bigger to the data
        if (largestFreeBuffer != nullptr)
        {
            largestFreeBuffer->data = (uint8_t *)realloc(largestFreeBuffer->data, size);
            if (!largestFreeBuffer->data)
            {
                //realloc failed, return nullptr
                return nullptr;
            }
            largestFreeBuffer->length = size;
            largestFreeBuffer->inUse = true;
            return largestFreeBuffer;
        }

        //no free buffer available, create a new one
        auto buffer = new Buffer();
        buffer->data = (uint8_t *)malloc(size);
        if (!buffer->data)
        {
            //malloc failed, return nullptr
            return nullptr;
        }
        buffer->length = size;
        buffer->inUse = true;
        buffers.push_back(buffer);
        return buffer;
    }
};

std::vector<Buffer*> BufferPool::buffers;

// class BufferPool
// {
// private:
//     static uint8_t * buffer;
//     static int bufferSize;

// public:
//     static uint8_t *getBuffer(int size)
//     {
//         if (size > bufferSize)
//         {
//             buffer = (uint8_t *)realloc(buffer, size);
//             bufferSize = size;
//         }
//         return buffer;
//     }
// };

// uint8_t* BufferPool::buffer = nullptr;
// int BufferPool::bufferSize = 0;