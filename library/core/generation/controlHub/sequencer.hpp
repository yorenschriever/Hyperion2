#pragma once

#include "IHubController.hpp"
#include "controlHub.hpp"
#include "generation/patterns/helpers/tempo/tempo.h"
#include "generation/patterns/helpers/tempo/tempoListener.h"
#include "log.hpp"
#include "misc/cjson/cJSON.h"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"

class Sequencer : public TempoListener
{
private:
    static const int STEPS = 64;
    static int indexCounter;

    class Sequence
    {
    public:
        int index;
        int columnIndex;
        int slotIndex;
        bool enabled = true;
        bool active[STEPS];
    };

    ControlHub *hub;
    std::unique_ptr<WebsocketServer> socket;
    std::vector<Sequence> sequences;
    int stepNr = 0;

    Sequence *getSequence(int index)
    {
        for (auto &sequence : sequences)
        {
            if (sequence.index == index)
                return &sequence;
        }
        return nullptr;
    }

    Sequence *getSequence(int columnIndex, int slotIndex)
    {
        for (auto &sequence : sequences)
        {
            if (sequence.columnIndex == columnIndex && sequence.slotIndex == slotIndex)
                return &sequence;
        }
        return nullptr;
    }

public:
    Sequencer(ControlHub *hub, WebServer *server)
    {
        this->hub = hub;
        Tempo::AddListener(this);

        socket = WebsocketServer::createInstance(server, "/ws/sequencer");
        socket->onMessage(handler, (void *)this);
        socket->onConnect(connectionHandler, (void *)this);
    }

    void OnBeat(int beatNr, const char *sourceName) override
    {
        this->stepNr = beatNr % STEPS;

        for (auto &sequence : sequences)
            updateHub(sequence);

        sendStepNr();
    }

    void updateHub(const Sequence &sequence)
    {
        hub->setSlotActive(sequence.columnIndex, sequence.slotIndex, sequence.enabled && sequence.active[stepNr], ControlHub::SEQUENCE);
    }

    static void connectionHandler(RemoteWebsocketClient *client, WebsocketServer *server, void *userData)
    {
        auto *instance = (Sequencer *)userData;
        instance->sendFullStatus();
    }

    static void handler(RemoteWebsocketClient *client, WebsocketServer *server, std::string msg, void *userData)
    {
        auto *instance = (Sequencer *)userData;
        cJSON *parsed = cJSON_Parse(msg.c_str());
        std::string type = cJSON_GetObjectItem(parsed, "type")->valuestring;

        if (type.compare("setSteps") == 0)
        {
            int sequenceIndex = cJSON_GetObjectItem(parsed, "sequenceIndex")->valueint;
            int startStep = cJSON_GetObjectItem(parsed, "startStep")->valueint;
            int endStep = cJSON_GetObjectItem(parsed, "endStep")->valueint;
            bool active = cJSON_GetObjectItem(parsed, "active")->valueint;
            instance->setSteps(sequenceIndex, startStep, endStep, active);
        }
        else if (type.compare("add") == 0)
        {
            int columnIndex = cJSON_GetObjectItem(parsed, "columnIndex")->valueint;
            int slotIndex = cJSON_GetObjectItem(parsed, "slotIndex")->valueint;
            // maybe i want to allow duplicates, so i can create different patterns, and toggle the one i need
            //but with the current code the second copy would override the first one. They need to be combined somehow
            if (instance->getSequence(columnIndex, slotIndex))
            {
                Log::error("Sequencer", "Sequence already exists for column %d slot %d", columnIndex, slotIndex);
                cJSON_Delete(parsed);
                return;
            }
            instance->sequences.push_back({.index = Sequencer::indexCounter++, .columnIndex = columnIndex, .slotIndex = slotIndex, .enabled = true});
            instance->sendSequenceDetails(instance->sequences.back());
        }
        else if (type.compare("remove") == 0)
        {
            int index = cJSON_GetObjectItem(parsed, "index")->valueint;
            auto sequence = instance->getSequence(index);
            if (!sequence)
            {
                Log::error("Sequencer", "No sequence found with index %d", index);
                cJSON_Delete(parsed);
                return;
            }
            instance->updateHub(*sequence);
            instance->sequences.erase(std::remove_if(instance->sequences.begin(), instance->sequences.end(), [index](const Sequence &s)
                                                     { return s.index == index; }),
                                      instance->sequences.end());
            instance->sendRemoveSequence(index);
        }
        else if (type.compare("enable") == 0)
        {
            int index = cJSON_GetObjectItem(parsed, "index")->valueint;
            int value = cJSON_GetObjectItem(parsed, "value")->valueint;
            auto sequence = instance->getSequence(index);
            if (!sequence)
            {
                Log::error("Sequencer", "No sequence found with index %d", index);
                cJSON_Delete(parsed);
                return;
            }
            sequence->enabled = value;
            instance->updateHub(*sequence);
            instance->sendSequenceStatus(*sequence);
        }

        cJSON_Delete(parsed);
    }

    void setSteps(int sequenceIndex, int startStep, int endStep, bool active)
    {
        Sequence *sequence = getSequence(sequenceIndex);
        if (!sequence)
            return;

        for (int step = startStep; step <= endStep; step++)
        {
            if (step < 0 || step >= STEPS)
                continue;
            sequence->active[step] = active;
        }

        updateHub(*sequence);
        sendSequenceStatus(*sequence);
    }

    void sendFullStatus()
    {
        for (auto &sequence : sequences)
        {
            sendSequenceDetails(sequence);
            sendSequenceStatus(sequence);
        }
        sendStepNr();
    }

    void sendSequenceDetails(const Sequence &sequence)
    {
        auto col = hub->findColumn(sequence.columnIndex);
        auto slot = hub->findSlot(sequence.columnIndex, sequence.slotIndex);

        if (!col || !slot)
            return;

        socket->sendAll(
            "{\
                \"type\":\"details\",\
                \"index\":%d,\
                \"enabled\":%d,\
                \"colName\":\"%s\",\
                \"slotName\":\"%s\"\
            }",
            sequence.index,
            sequence.enabled,
            col->name.c_str(),
            slot->name.c_str());
    }

    void sendSequenceStatus(const Sequence &sequence)
    {
        char steps[STEPS + 1];
        for (int i = 0; i < STEPS; i++)
            steps[i] = sequence.active[i] ? '1' : '0';
        steps[STEPS] = '\0';

        socket->sendAll(
            "{\
                \"type\":\"status\",\
                \"index\":%d,\
                \"enabled\":%d,\
                \"steps\":\"%s\"\
            }",
            sequence.index,
            sequence.enabled,
            steps);
    }

    void sendRemoveSequence(int sequenceIndex)
    {
        socket->sendAll(
            "{\
                \"type\":\"remove\",\
                \"index\":%d\
            }",
            sequenceIndex);
    }

    void sendStepNr()
    {
        socket->sendAll(
            "{\
                \"type\":\"stepNr\",\
                \"stepNr\":%d\
            }",
            stepNr);
    }
};

int Sequencer::indexCounter = 0;