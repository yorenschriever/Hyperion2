#pragma once

class Network
{
public:
    static void setHostName(const char* hostname);
    static const char* getHostName();

private:
    static const char* hostname;
};
