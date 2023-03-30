#pragma once

#include "output.hpp"
#include "core/distribution/utils/hostnameCache.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/socket.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include <algorithm>
#include <vector>


class CloneOutput : public Output
{
public:
    CloneOutput(std::vector<Output*> destinations)
    {
        this->destinations = destinations;
    }

    void setData(uint8_t *data, int size, int index) override
    {
        for(auto destination : destinations)
            destination->setData(data,size,index);
    }

    bool ready() override
    {
        for(auto destination : destinations)
            if (!destination->ready())
                return false;
        return true;
    }

    void show() override
    {
        for(auto destination : destinations)
            destination->show();
    }

    void postProcess() override
    {
        for(auto destination : destinations)
            destination->postProcess();
    }

    void begin() override
    {
        for(auto destination : destinations)
            destination->begin();
    }

    void clear() override
    {
        for(auto destination : destinations)
            destination->clear();
    }

    // length is in bytes
    void setLength(int len) override
    {
         for(auto destination : destinations)
            destination->setLength(len);
    }

protected:
    std::vector<Output*> destinations;
};
