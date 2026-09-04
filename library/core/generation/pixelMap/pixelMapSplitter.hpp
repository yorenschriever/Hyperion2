// #pragma once

// #include <algorithm>
// #include <cstring>
// #include <vector>
// #include "pixelMap.hpp"

// class PixelMapSplitter
// {
// public:
//     PixelMapSplitter(PixelMapPtrsourceMap, std::vector<int> lengths)
//     {
//         this->sourceMap = sourceMap;

//         auto start = sourceMap->begin();
//         for (int length : lengths)
//         {

//             PixelMap map(length);
//             copy(start, start+length, map.begin());
//             destinationMaps.push_back(map);

//             start += length;
//         }
//     }

//     PixelMapPtrgetMap(int index)
//     {
//         return &(destinationMaps[index]);
//     }

//     int size()
//     {
//         return destinationMaps.size();
//     }

// private:
//     PixelMapPtrsourceMap = NULL;
//     std::vector<PixelMap> destinationMaps;

// };
