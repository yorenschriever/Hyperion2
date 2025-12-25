#pragma once

#include "switch.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>

class Fallback final : public Switch
{
public:
    Fallback(int size, int timeout = 500) : Switch(size)
    {
        this->switchFunc = [this, timeout]()
        {
            int index = 0;
            for (auto recv : inReceivers)
            {
                if (Utils::millis() - recv->lastSignal > timeout)
                {
                    return index;
                    index++;
                }
            }
            return (int)inReceivers.size() - 1;
        };
    }
};
