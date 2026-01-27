#pragma once

#include "../interfaces.hpp"
#include "log.hpp"
#include <algorithm>
#include <cstring>
#include <functional>
#include <vector>

class Fallback;

// TODO docs
// you can either provide a switchfunc or use setReceiver.
class Switch : public ISource
{

    class SwitchReceiver : public ISink
    {
    public:
        SwitchReceiver(Switch *parent)
        {
            this->parent = parent;
        }

        void initialize() override {}

        bool ready() override
        {
            lastSignal = Utils::millis();
            return !parent->dirty;
        }

        void process(const Buffer inputBuffer) override
        {
            if (!active)
                return;
            parent->buffer = inputBuffer;
            parent->dirty = true;
        }

    private:
        bool active;
        Switch *parent;
        unsigned long lastSignal = 0;

        friend Switch;
        friend Fallback;
    };

public:
    using SwitchFunc = std::function<int()>;

    Switch(int size, SwitchFunc switchFunc = nullptr)
    {
        for (int i = 0; i < size; i++)
        {
            inReceivers.push_back(new SwitchReceiver(this));
        }
        this->switchFunc = switchFunc;
    }

    ISink *getReceiver(int i)
    {
        if (i >= inReceivers.size())
        {
            Log::error("SWITCH", "Switch.getReceiver(%d) out of bounds (%d)", i, inReceivers.size());
            return nullptr;
        }

        return inReceivers[i];
    }

    void setReceiver(int receiverIndex)
    {
        if (receiverIndex >= inReceivers.size())
        {
            Log::error("SWITCH", "Switch.setReceiver(%d) out of bounds (%d)", receiverIndex, inReceivers.size());
        }

        int i = 0;
        for (auto rcv : inReceivers)
        {
            rcv->active = receiverIndex == i;
            i++;
        }
    }

    void initialize() override
    {
    } 

    bool ready() override
    {
        return dirty;
    }

    Buffer process() override
    {
        checkSwitchFunc();
        dirty = false;
        return buffer;
    }

protected:
    std::vector<SwitchReceiver *> inReceivers;
    SwitchFunc switchFunc;
    bool dirty = false;
    Buffer buffer = Buffer(0);

    void checkSwitchFunc()
    {
        if (!switchFunc)
            return;
        setReceiver(switchFunc());
    }
};
