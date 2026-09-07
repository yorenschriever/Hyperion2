#pragma once
#include "hyperion.hpp"
#include "webServer.hpp"
#include "webServerResponseBuilder.hpp"

const char viewParamsFmt[] = "\n\
export const defaultOrbit = {\n\
    azimuth: %.4f,\n\
    elevation: %.4f,\n\
    distance: %.4f,\n\
    panY: %.4f,\n\
}\n\
\n\
export const viewParams = {\n\
    fieldOfView: (%.4f * Math.PI) / 180,\n\
    gridZ: %.4f,\n\
    orbit: { ...defaultOrbit },\n\
};\n";

class ViewParams : public WebServerResponseBuilder
{
    char viewParams[400];
    int size;

public:
    ViewParams(
        float fieldOfView,
        float gridZ,
        float distance,
        float azimuth,
        float elevation,
        float panY = 0)
    {
        size = snprintf(viewParams, sizeof(viewParams), viewParamsFmt,
                        azimuth, elevation, distance, panY,
                        fieldOfView, gridZ);
    }

    void build(Writer write, std::string postBody, void *userData) override
    {
        write(viewParams, size, userData);
    }
};

void setViewParams(Hyperion *hyp, ViewParams *vp)
{
    hyp->webServer->addPath("/monitor/view-params.js", vp);
}

auto viewParamsDefault = new ViewParams(80, -0.45, 2, 0, -.1);
auto viewParamsTop = new ViewParams(20, -0.35, 5.5, 0, 3.14/2);
auto viewParamsFront = new ViewParams(20, -0.35, 5.5, 0, 0);
auto viewParamsSide = new ViewParams(20, -0.35, 5.5, 3.14/2, 0);
