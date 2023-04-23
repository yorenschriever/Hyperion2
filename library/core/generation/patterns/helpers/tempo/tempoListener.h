#pragma once

class TempoListener
{
public:
    virtual void OnBeat(int beatNr, const char* sourceName)=0;    
};