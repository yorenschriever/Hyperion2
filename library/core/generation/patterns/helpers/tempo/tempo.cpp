#include "utils.hpp"
#include "thread.hpp"
#include "tempo.h"
//#include "udpTempo.h"
#include "abstractTempo.h"


void Tempo::BroadcastBeat(AbstractTempo *source)
{
    // if (getActive() != source)
    //     return;
    // UdpTempo::getInstance()->broadcastBeat();
}

void Tempo::AddSource(AbstractTempo *source)
{
    sources.insert(source);
    registerTask(source->Initialize());
}

int Tempo::GetBeatNumber()
{
    AbstractTempo *source = getActive();
    if (source)
        return source->GetBeatNumber() + phraseOffset;
    return -1;
};

int Tempo::TimeBetweenBeats()
{
    AbstractTempo *source = getActive();
    if (source)
        return source->TimeBetweenBeats();
    return 0;
};

const char *Tempo::SourceName()
{
    AbstractTempo *source = getActive();
    if (source)
        return source->Name();
    return "None";
}

float Tempo::GetProgress(int intervalSize)
{
    AbstractTempo *source = getActive();
    if (!source)
        return 0;

    if (source->TimeBetweenBeats() == 0)
        return 0;

    float coarse = float((source->GetBeatNumber() + phraseOffset) % intervalSize);
    float fine = float(Utils::millis() - source->TimeOfLastBeat()) / float(source->TimeBetweenBeats());
    float result = (coarse + fine) / intervalSize;

    if (result < 0)
        return 0;
    if (result > 1)
        return 1;
    return result;
}

AbstractTempo *Tempo::getActive()
{
    for (auto &&source : sources)
        if (source->HasSignal())
            return source;
    return nullptr;
}

void Tempo::TempoTask(void *param)
{
    while (true)
    {
        for (auto &&task : tasks)
            task();
        Thread::sleep(1);
    }
}

void Tempo::registerTask(TempoTaskType task)
{
    if (task == nullptr)
        return;

    tasks.insert(task);
    if (!threadStarted){
        Thread::create(TempoTask, "TempoTask", Thread::Purpose::generation, 5000,nullptr,0);
        threadStarted = true;
    }
}

void Tempo::AlignPhrase()
{
    // if this function is called this means that the current bar was the start of a phrase
    // realign the beat counter
    int beatnr = GetBeatNumber();
    int barnr = (beatnr / 4) % 8;
    // i dont want negative beatnumbers. always correct forwards, add 4*8.
    phraseOffset = (phraseOffset - 4 * barnr + 4 * 8) % 32;
}

std::set<AbstractTempo *> Tempo::sources;
std::set<TempoTaskType> Tempo::tasks;
bool Tempo::threadStarted = false;
int Tempo::phraseOffset = 0;
