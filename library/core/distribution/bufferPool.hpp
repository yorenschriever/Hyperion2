#pragma once
#include "log.hpp"
#include <cstdlib>
#include <inttypes.h>
#include <mutex>
#include <vector>

// This is written for use in the distribution module only.
// If this is going to be called from outside the distribution module, it should be rewritten to be thread safe.
class BufferPool;

class Buffer
{
    friend BufferPool;

public:
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
    int length;   // Length is the length as requested by the user. This is also publicly available
    int capacity; // Capacity is only used internally to determine if the buffer is big enough when reusing it
    bool inUse = false;
};

class BufferPool
{
private:
    static std::vector<Buffer *> buffers;
    static std::mutex mutex;

public:
    static Buffer *getBuffer(int size)
    {
        mutex.lock();
        // Log::info("BUFFER_POOL", "Requested buffer of size %d. (%d buffers available)", size, buffers.size());
        // for (int i = 0; i < buffers.size(); i++)
        // {
        //     Log::info("BUFFER_POOL", "Buffer %d: capacity %d, inUse=%d", i, buffers[i]->capacity, buffers[i]->inUse);
        // }

        if (buffers.size() > 20){
            Log::error("BUFFERPOOL","Bufferpool > 20, sus");
        }

        Buffer *largestFreeBuffer = nullptr;
        Buffer *smallestSuitableBuffer = nullptr;

        // see if a free buffer is available
        for (auto buffer : buffers)
        {
            if (buffer->inUse)
                continue;

            if (
                buffer->capacity >= size && 
                (smallestSuitableBuffer == nullptr || buffer->capacity < smallestSuitableBuffer->capacity))
            {
                smallestSuitableBuffer = buffer;
            }

            if (largestFreeBuffer == nullptr || buffer->capacity > largestFreeBuffer->capacity)
            {
                largestFreeBuffer = buffer;
            }
        }

        if (smallestSuitableBuffer != nullptr)
        {
            // Log::info("BUFFER_POOL", "Found suitable buffer of capacity %d", smallestSuitableBuffer->capacity);
            smallestSuitableBuffer->inUse = true;
            smallestSuitableBuffer->length = size;
            mutex.unlock();
            return smallestSuitableBuffer;
        }

        // make the largest buffer a bit bigger to the data
        if (largestFreeBuffer != nullptr)
        {
            // Log::info("BUFFER_POOL", "No suitable buffer found in %d buffers. Resizing the largest one from %d to %d", buffers.size(), largestFreeBuffer->capacity, size);

            largestFreeBuffer->data = (uint8_t *)realloc(largestFreeBuffer->data, size);
            if (!largestFreeBuffer->data)
            {
                // realloc failed, return nullptr
                mutex.unlock();
                buffers.erase(std::remove(buffers.begin(), buffers.end(), largestFreeBuffer), buffers.end());
                return nullptr;
            }
            largestFreeBuffer->capacity = size;
            largestFreeBuffer->length = size;
            largestFreeBuffer->inUse = true;
            mutex.unlock();
            return largestFreeBuffer;
        }

        // Log::info("BUFFER_POOL", "No free buffer found in %d buffers. Allowcating new one", buffers.size());

        // no free buffer available, create a new one
        auto buffer = new Buffer();
        buffer->data = (uint8_t *)malloc(size);
        if (!buffer->data)
        {
            // malloc failed, return nullptr
            mutex.unlock();
            return nullptr;
        }
        buffer->capacity = size;
        buffer->length = size;
        buffer->inUse = true;
        buffers.push_back(buffer);
        mutex.unlock();
        return buffer;
    }

    static void release(Buffer *buffer)
    {
        // Log::info("BUFFER_POOL", "Releasing buffer of capacity %d", buffer->capacity);
        mutex.lock();
        buffer->length = 0;
        buffer->inUse = false;
        mutex.unlock();
    }
};

std::vector<Buffer *> BufferPool::buffers;
std::mutex BufferPool::mutex;
