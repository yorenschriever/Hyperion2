#pragma once
#include "../interfaces.hpp"
#include "log.hpp"
#include <vector>

class CombinedInput;

class ICombinedInput {
    public:
    virtual void setLengthCombined()=0;
    virtual void showCombined()=0;
    virtual bool readyCombined()=0;
    virtual void setDataCombined(uint8_t *data, int size, int index)=0;
};

class CombinedInputPart final : public IOutput
{
friend CombinedInput;
public:
    CombinedInputPart(ICombinedInput *parent, int offset, bool sync){
        this->parent = parent;
        this->offset = offset;
        this->sync = sync;
    }

    void setData(uint8_t *data, int size) override {
        parent->setDataCombined(data, size, offset);
    }

    void setLength(int len) override {
        length = len;
        parent->setLengthCombined();
    }

    void show() override {
        dirty = true;
        parent->showCombined();
    }



    bool ready() override {
        return parent->readyCombined();
    }

    void begin() override { 
        //nothing to do here.
    }

    void clear() override {
        //nothing to do here. 
    }

private:
    ICombinedInput *parent;
    int offset;
    int length=0;
    bool dirty = false;
    bool sync;
};

// CombinedInput reads the input from multiple sources and glues them together
class CombinedInput final : public IInput, ICombinedInput
{
friend CombinedInputPart;

public:

    CombinedInputPart *atOffset(int offset, bool sync = true)
    {
        auto part = new CombinedInputPart(this, offset, sync);
        parts.push_back(part);
        return part;
    }

    void setReceiver(IReceiver *receiver) override {
        this->receiver = receiver;
    }

    void process() override {}

private:
    IReceiver *receiver = nullptr;
    std::vector<CombinedInputPart*> parts;
    uint8_t *buffer = nullptr;
    int bufferSize=0;

    void begin() override {}

    bool readyCombined() override {
        return receiver && receiver->ready();
    }   

    void setLengthCombined() override{
        int maxLength = 0;
        for (auto part : parts)
            maxLength = std::max(maxLength, part->offset + part->length);

        buffer = (uint8_t*)realloc(buffer, maxLength);
        bufferSize = maxLength;
    }

    void showCombined() override{
        for (auto part : parts)
            if (part->sync && !part->dirty)
                return;

        if (!receiver)
            return;

        receiver->setLength(bufferSize);
        receiver->setData(buffer, bufferSize);
        receiver->show();

        for (auto part : parts)
            part->dirty = false;
    }

    void setDataCombined(uint8_t *data, int size, int index) override {
        int safeSize = std::min(size, bufferSize - index);
        memcpy(buffer + index, data, safeSize);
    }
};