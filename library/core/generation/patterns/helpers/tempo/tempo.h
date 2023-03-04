#pragma once

#include "utils.hpp"
#include <set>

using TempoTaskType = void (*)();

class AbstractTempo;

class Tempo
{
public:
    static void AddSource(AbstractTempo *source);

    static int GetBeatNumber();

    static int TimeBetweenBeats();

    static const char *SourceName();

    //gets a value between 0-1 that increases linearly in the given interval
    //eg intervalSize=1  gives a value that fills between every beat
    //   intervalSize=4  gives a value that fills between every measure
    //   intervalSize=16 gives a value that fills between every phrase
    static float GetProgress(int intervalSize);

    static void BroadcastBeat(AbstractTempo *source);

    static void AlignPhrase();
private:
    
    static std::set<AbstractTempo *> sources;
    static std::set<TempoTaskType> tasks;
    static bool threadStarted;
    static TempoTaskType broadcastTask;

    static void TempoTask(void *param);
    static AbstractTempo *getActive();
    static void registerTask(TempoTaskType task);

    static int phraseOffset;
};