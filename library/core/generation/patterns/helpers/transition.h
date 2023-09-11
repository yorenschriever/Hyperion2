#pragma once
#include "fade.h"

using FadeShape = int(*)(float, float, int);

class Transition
{
 
public:
    //use this class to animate specific animations at the start and the end of 
    //a pattern.
    //mostly used to fade in/out layered patterns 
    Transition(
        unsigned int fadeInDuration,
        FadeShape fadeInShape,
        unsigned int fadeInShapeSize,
        unsigned int fadeOutDuration,
        FadeShape fadeOutShape,
        unsigned int fadeOutShapeSize
    )
    {
        fadein.duration = fadeInDuration;
        fadeout.duration = fadeOutDuration;
        this->fadeInShapeSize = fadeInShapeSize;
        this->fadeOutShapeSize = fadeOutShapeSize;
        this->fadeInShape = fadeInShape;
        this->fadeOutShape = fadeOutShape;
        lastActive=false;
    }

    Transition(
        unsigned int fadeInDuration=200,
        unsigned int fadeOutDuration=200
    )
    {
        fadein.duration = fadeInDuration;
        fadeout.duration = fadeOutDuration;
        this->fadeInShapeSize = 0;
        this->fadeOutShapeSize = 0;
        this->fadeInShape = none;
        this->fadeOutShape = none;
        lastActive=false;
    }

    bool Calculate(bool active){
        if (active && !lastActive)
            fadein.reset();
        if (!active && lastActive)
            fadeout.reset();
        lastActive = active;

        //let the pattern know if getValue is going to return anything other than 0,
        //if not, the pattern can early exit pattern calculation
        return active || fadeout.getValue(fadeOutShapeSize) > 0;
    }

    float getValue(){
        return fadein.getValue() * (lastActive?1:fadeout.getValue());
    }

    float getValue(float position, float width){
        return fadein.getValue(fadeInShape(position,width,fadeInShapeSize)) * (lastActive?1:fadeout.getValue(fadeOutShape(position,width,fadeOutShapeSize)));
    }

    static int none(float position, float width, int delay){ return 0;}
    static int fromEnd(float position, float width, int delay){ return delay * position / width;}
    static int fromStart(float position, float width, int delay){return delay * (width-position) / width;}
    static int fromCenter(float position, float width, int delay){return (delay * std::abs(width - 2*position)) / width;}
    static int fromSides(float position, float width, int delay){return (delay * std::abs(2*position-width)) / width;}

private:
    Fade<Up> fadein= Fade<Up> (0, WaitAtStart);
    Fade<Down> fadeout= Fade<Down>(0, WaitAtStart);
    bool lastActive;

    int fadeInShapeSize;
    int fadeOutShapeSize;

    FadeShape fadeInShape=none;
    FadeShape fadeOutShape=none;
};