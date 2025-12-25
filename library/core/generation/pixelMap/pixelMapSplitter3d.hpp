#pragma once

#include <algorithm>
#include <cstring>
#include <vector>
#include "pixelMap.hpp"

class PixelMapSplitter3d
{
public:
    PixelMapSplitter3d(PixelMap3d *sourceMap, std::vector<int> lengths)
    {
        this->sourceMap = sourceMap;

        auto start = sourceMap->begin();
        for (int length : lengths)
        {

            PixelMap3d map(length);
            copy(start, start+length, map.begin());
            destinationMaps.push_back(map);

            start += length;
        }
    }

    PixelMap3d *getMap(int index)
    {
        return &(destinationMaps[index]);
    }

    int size()
    {
        return destinationMaps.size();
    }

private:
    PixelMap3d *sourceMap = NULL;
    std::vector<PixelMap3d> destinationMaps;

};
