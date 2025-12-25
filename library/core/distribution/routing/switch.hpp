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
class Switch : public ISender
{

    class SwitchReceiver : public IReceiver
    {
    public:
        SwitchReceiver(Switch *parent)
        {
            this->parent = parent;
        }

        bool ready() override
        {
            lastSignal = Utils::millis();
            parent->checkSwitchFunc();
            if (!active)
                return false;
            if (!parent->outReceiver)
                return false;
            return parent->outReceiver->ready();
        }

        void setLength(int length) override
        {
            if (!active)
                return;
            if (!parent->outReceiver)
                return;
            parent->outReceiver->setLength(length);
        }

        void setData(uint8_t *data, int size) override
        {
            if (!active)
                return;
            if (!parent->outReceiver)
                return;
            parent->outReceiver->setData(data, size);
        }

        void show() override
        {
            if (!active)
                return;
            if (!parent->outReceiver)
                return;
            parent->outReceiver->show();
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

    IReceiver *getReceiver(int i)
    {
        if (i >= inReceivers.size())
        {
            Log::error("", "Switch.getReceiver(%d) out of bounds (%d)", i, inReceivers.size());
            return nullptr;
        }

        return inReceivers[i];
    }

    void setReceiver(IReceiver *receiver) override
    {
        this->outReceiver = receiver;
    }

    void setReceiver(int receiverIndex)
    {
        if (receiverIndex >= inReceivers.size())
        {
            Log::error("", "Switch.setReceiver(%d) out of bounds (%d)", receiverIndex, inReceivers.size());
        }

        int i = 0;
        for (auto rcv : inReceivers)
        {
            rcv->active = receiverIndex == i;
            i++;
        }
    }

protected:
    IReceiver *outReceiver;
    std::vector<SwitchReceiver *> inReceivers;
    SwitchFunc switchFunc;

    void checkSwitchFunc()
    {
        if (!switchFunc)
            return;
        setReceiver(switchFunc());
    }
};
