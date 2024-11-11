#include "utils.hpp"
#include "thread.hpp"
#include "tempo.h"
//#include "udpTempo.h"
#include "abstractTempo.h"
#include "log.hpp"

static const char* TAG = "TEMPO";

void Tempo::BroadcastBeat(AbstractTempo *source)
{
    //if we got a beat from a source that is currently not 
    //selected as the active source, we ignore it
    if (getActive() != source)
        return;

    const char* sourceName = SourceName();
    int bearNr = GetBeatNumber();
    for (auto listener : listeners)
        listener->OnBeat(bearNr, sourceName);
    // UdpTempo::getInstance()->broadcastBeat();
}

void Tempo::AddSource(AbstractTempo *source)
{
    sources.insert(source);
    startTaskThread();
}

void Tempo::RemoveSource(AbstractTempo *source)
{
    sources.erase(source);
}

void Tempo::AddListener(TempoListener *listener)
{
    listeners.insert(listener);
}

void Tempo::RemoveListener(TempoListener *listener)
{
    listeners.erase(listener);
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
    if (intervalSize == 0)
        return 0;

    AbstractTempo *source = getActive();
    if (!source)
        return 0;

    if (source->TimeBetweenBeats() == 0)
        return 0;

    if (source->GetBeatNumber() < 0)
        return 0;

    float coarse = float((source->GetBeatNumber() + phraseOffset) % intervalSize);
    float fine = float(Utils::millis() - source->TimeOfLastBeat()) / float(source->TimeBetweenBeats());
    float result = (coarse + fine) / intervalSize;

    // Log::info("Tempo", "beat: %d, last beat: %lu, time between beats: %d", source->GetBeatNumber(), source->TimeOfLastBeat(), source->TimeBetweenBeats());
    // Log::info("Tempo", "coarse: %f, fine: %f, result: %f", coarse, fine, result);

    return Utils::constrain_f(result, 0, 1);
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
        for (auto source : sources)
            source->TempoTask();
        Thread::sleep(1);
    }
}

void Tempo::startTaskThread()
{
    if (!threadStarted){
        Log::info(TAG, "Create tempo task");
        Thread::create(TempoTask, "TempoTask", Thread::Purpose::control, 5000,nullptr,1);
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
std::set<TempoListener *> Tempo::listeners;
bool Tempo::threadStarted = false;
int Tempo::phraseOffset = 0;
