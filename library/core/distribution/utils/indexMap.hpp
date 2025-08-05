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
