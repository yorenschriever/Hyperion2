#pragma once

#include "controlHub.hpp"
#include "IHubController.hpp"
#include "generation/patterns/helpers/tempo/tempoListener.h"
#include "generation/patterns/helpers/tempo/tempo.h"
#include "log.hpp"
#include "platform/includes/websocketServer.hpp"
#include "platform/includes/webServer.hpp"
#include "misc/cjson/cJSON.h"

class Sequencer : public TempoListener
{
    private:
        static const int STEPS = 64;
        static int indexCounter;

        class Sequence {
            public:
            int index;
            int columnIndex;
            int slotIndex;
            bool active[STEPS];
        };

        ControlHub *hub;
        std::unique_ptr<WebsocketServer> socket;
        std::vector<Sequence> sequences;
        int stepNr = 0;

    public:
        Sequencer(ControlHub *hub, WebServer *server)
        {
            this->hub = hub;
            Tempo::AddListener(this);

            socket = WebsocketServer::createInstance(server,"/ws/sequencer");
            socket->onMessage(handler, (void *)this);
            socket->onConnect(connectionHandler, (void *)this);

            //TODO dynamically add/remove sequences
            for(int i=0;i<hub->getColumnCount();i++){
                for(int j=0;j<hub->getSlotCount(i);j++){
                    sequences.push_back({.index = indexCounter++, .columnIndex = i, .slotIndex = j});
                }
            }
        }

        void OnBeat(int beatNr, const char* sourceName) override
        {
            this->stepNr = beatNr % STEPS;

            for (auto &sequence : sequences)
            {
                //todo only update on change -> compared to status in the hub, because beats are not linear, and steps can be edited
                //todo special mode
                //todo edges? start and end. think about it
                if (sequence.active[stepNr])
                {
                    hub->setSlotActive(sequence.columnIndex, sequence.slotIndex, true);
                }
                if (!sequence.active[stepNr])
                {
                    hub->setSlotActive(sequence.columnIndex, sequence.slotIndex, false);
                }
            }

            sendStepNr();
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
        std::string type = cJSON_GetObjectItem(parsed,"type")->valuestring;

        if (type.compare("setSteps") == 0)
        {
            int sequenceIndex = cJSON_GetObjectItem(parsed,"sequenceIndex")->valueint;
            int startStep = cJSON_GetObjectItem(parsed,"startStep")->valueint;
            int endStep = cJSON_GetObjectItem(parsed,"endStep")->valueint;
            bool active = cJSON_GetObjectItem(parsed,"active")->valueint;
            instance->setSteps(sequenceIndex, startStep, endStep, active);
        }
        
        cJSON_Delete(parsed);
    }

    void setSteps(int sequenceIndex, int startStep, int endStep, bool active)
    {
        if (sequenceIndex < 0 || sequenceIndex >= sequences.size())
            return;

        for (int step = startStep; step <= endStep; step++)
        {
            if (step < 0 || step >= STEPS)
                continue;
            sequences[sequenceIndex].active[step] = active;
        }

        sendSequenceStatus(sequenceIndex);
    }

    void sendFullStatus()
    {
        for (int i = 0; i < sequences.size(); i++){
            sendSequenceDetails(i);
            sendSequenceStatus(i);
        }
        sendStepNr();
    }

    void sendSequenceDetails(int sequenceIndex){
        if (sequenceIndex < 0 || sequenceIndex >= sequences.size())
            return;
        auto &sequence = sequences[sequenceIndex];
        
        auto col = hub->findColumn(sequence.columnIndex);
        auto slot = hub->findSlot(sequence.columnIndex, sequence.slotIndex);

        if (!col || !slot)
            return;

        socket->sendAll(
            "{\
                \"type\":\"details\",\
                \"index\":%d,\
                \"colName\":\"%s\",\
                \"slotName\":\"%s\"\
            }",
            sequenceIndex,
            col->name.c_str(),
            slot->name.c_str());
    }

    void sendSequenceStatus(int sequenceIndex)
    {
        if (sequenceIndex < 0 || sequenceIndex >= sequences.size())
            return;
        auto &sequence = sequences[sequenceIndex];
        
        char steps[STEPS+1];
        for (int i = 0; i < STEPS; i++)
            steps[i] = sequence.active[i] ? '1' : '0';  
        steps[STEPS] = '\0';

        socket->sendAll(
            "{\
                \"type\":\"status\",\
                \"index\":%d,\
                \"steps\":\"%s\"\
            }",
            sequenceIndex,
            steps);
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