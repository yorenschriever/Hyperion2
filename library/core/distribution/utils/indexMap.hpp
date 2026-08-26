#pragma once

class IndexMap
{
    public:
        virtual int map(int)=0;
};

class ZigZagMapper : public IndexMap
{
    int segmentSize;
    bool flip;
public:
    ZigZagMapper(int segmentSize = 60, bool flip = false)
    {
        this->segmentSize = segmentSize;
        this->flip = flip;
    }

    int map(int i) override
    {
        //zig
        if ((i % (2 * segmentSize) < segmentSize) ^ flip)
            return i;
        //zag
        int start = int(i / (segmentSize)) * (segmentSize);
        return start + segmentSize - 1 - (i % segmentSize);
    }
};

class ReverseMapper : public IndexMap
{
    int size;

public:
    ReverseMapper(int size)
    {
        this->size = size;
    }

    int map(int i) override
    {
        return size - 1 - i;
    }
};

class FlipMapper : public IndexMap
{
    std::vector<int> mappedIndices;
public:
    FlipMapper(int patternSize){
        for(int i = 0; i < patternSize; i++)
            mappedIndices.push_back(i);
    }

    FlipMapper *flip(int startIndex, int chunkSize)
    {
        for(int i = 0; i < chunkSize; i++)
        {
            mappedIndices[startIndex + i] = startIndex + chunkSize - 1 - i;
        }
        return this;
    }

    FlipMapper *rotate(int rotationAmount)
    {
        std::vector<int> rotatedIndices(mappedIndices.size());
        int size = mappedIndices.size();
        for(int i = 0; i < size; i++)
        {
            rotatedIndices[(i + rotationAmount) % size] = mappedIndices[i];
        }
        mappedIndices = rotatedIndices;
        return this;
    }

    int map(int i) override
    {
        int size = mappedIndices.size();
        int modIndex = i % size;
        int repeatIndex = i / size;
        return mappedIndices[modIndex] + repeatIndex * size;
    }
};