#pragma once

#include "ethernet.hpp"

class Build
{
public:
    static const char* getProjectName() { 
        return PROJECT_NAME;
    }
    static const char* getBuildTime() {
        return BUILD_TIME;
    }
    static const char* getBuildId(){
        snprintf(buildId, sizeof(buildId),"%s/%x%x%x%x%x%x",
        getBuildTime(),
            Ethernet::getMac().octets[0],
            Ethernet::getMac().octets[1],
            Ethernet::getMac().octets[2],
            Ethernet::getMac().octets[3],
            Ethernet::getMac().octets[4],
            Ethernet::getMac().octets[5]);
        return buildId;
    }
private:
    static char buildId[30];
};

char Build::buildId[30];