#pragma once
#include "hyperion.hpp"
#include "webServer.hpp"
#include "webServerResponseBuilder.hpp"

class ViewParams : public WebServerResponseBuilder
{
    std::string viewParams;

public:
    ViewParams(std::string viewParams)
    {
        this->viewParams = viewParams;
    }

    void build(Writer write, void *userData) override
    {
        write(this->viewParams.c_str(), viewParams.length(), userData);
    }
};

// top view
ViewParams viewParamsTop = ViewParams("\n\
export const viewParams = {\n\
    clearColor: [0.1, 0.1, 0.1, 1.0],\n\
    fieldOfView: (80 * Math.PI) / 180,\n\
    gridZ: -0.35,\n\
    transform: [\n\
        {\n\
            //move 2 units back\n\
            type:'translate',\n\
            amount: [0,0,-1.5]\n\
        },\n\
        {\n\
            // look slightly from below\n\
            type:'rotate',\n\
            amount: (_t)=>90/360*2*3.1415,\n\
            vector:[1,0,0]\n\
        },\n\
        {\n\
            //rotate the scene\n\
            type:'rotate',\n\
            amount: (t)=>0,\n\
            vector: [0,1,0]\n\
        }\n\
    ]\n\
};\n");

// Front view
ViewParams viewParamsFront = ViewParams("\n\
export const viewParams = {\n\
    clearColor: [0.1, 0.1, 0.1, 1.0],\n\
    fieldOfView: (80 * Math.PI) / 180,\n\
    gridZ: -0.35,\n\
    transform: [\n\
        {\n\
            //move back\n\
            type:'translate',\n\
            amount: [0,-0.1,-1.9]\n\
        },\n\
        {\n\
            // look slightly from above\n\
            type:'rotate',\n\
            amount: (_t)=>0.2,\n\
            vector:[1,0,0]\n\
        },\n\
        {\n\
            //rotate the scene\n\
            type:'rotate',\n\
            amount: (t)=>0,\n\
            vector: [0,1,0]\n\
        }\n\
    ]\n\
};\n");

void setViewParams(Hyperion *hyp, ViewParams *vp)
{
    hyp->webServer->addPath("/monitor/view-params.js", vp);
}
