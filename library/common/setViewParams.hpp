#pragma once
#include "hyperion.hpp"
#include "webServer.hpp"
#include "webServerResponseBuilder.hpp"

const char viewParamsFmt[] = "\n\
export const viewParams = {\n\
    fieldOfView: (%.4f * Math.PI) / 180,\n\
    gridZ: %.4f,\n\
    transform: [\n\
        {\n\
            //move 2 units back\n\
            type:'translate',\n\
            amount: [%.4f,%.4f,%.4f]\n\
        },\n\
        {\n\
            // look slightly from below\n\
            type:'rotate',\n\
            amount: (_t)=>%.4f,\n\
            vector:[%.4f,%.4f,%.4f]\n\
        },\n\
        {\n\
            //rotate the scene\n\
            type:'rotate',\n\
            amount: (t)=>t*%.4f,\n\
            vector: [%.4f,%.4f,%.4f]\n\
        }\n\
    ]\n\
};\n";

typedef float Vector[3];
typedef struct
{
    float amount;
    Vector vector;
} Rotation;

class ViewParams : public WebServerResponseBuilder
{
    char viewParams[700];
    int size;

public:
    ViewParams(
        float fieldOfView,
        float gridZ,
        Vector transform,
        Rotation rotateStatic,
        Rotation rotateAnimated)
    {
        size = snprintf(viewParams, sizeof(viewParams), viewParamsFmt,
                        fieldOfView, gridZ,
                        transform[0], transform[1], transform[2],
                        rotateStatic.amount,
                        rotateStatic.vector[0], rotateStatic.vector[1], rotateStatic.vector[2],
                        rotateAnimated.amount,
                        rotateAnimated.vector[0], rotateAnimated.vector[1], rotateAnimated.vector[2]);
    }

    void build(Writer write, void *userData) override
    {
        write(viewParams, size, userData);
    }
};

void setViewParams(Hyperion *hyp, ViewParams *vp)
{
    hyp->webServer->addPath("/monitor/view-params.js", vp);
}

auto viewParamsDefault = new ViewParams(80, -0.45, Vector{0, 0, -2}, Rotation{-.1, 1, 0, 0}, Rotation{0.125, 0, 1, 0});
auto viewParamsTop = new ViewParams(20, -0.35, Vector{0, 0, -5.5}, Rotation{3.14/2, 1, 0, 0}, Rotation{0, 0, 0, 0});
auto viewParamsFront = new ViewParams(20, -0.35, Vector{0, 0, -5.5}, Rotation{0, 1, 0, 0}, Rotation{0, 0, 0, 0});
auto viewParamsSide = new ViewParams(20, -0.35, Vector{0, 0, -5.5}, Rotation{3.14/2, 0, 1, 0}, Rotation{0, 0, 0, 0});
