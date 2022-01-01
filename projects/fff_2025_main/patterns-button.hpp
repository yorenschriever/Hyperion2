#pragma once
#include "core/generation/patterns/helpers/tempo/tempoListener.h"
#include <algorithm> // std::count

namespace Buttons
{    
    class RedButtonManager : public TempoListener
    {
    private:
        std::vector<bool> pressed = {false, false, false, false, false, false};
        float alignmentStart = 0;
        const int alignTo = 4; // Align to measure (4 beats)

        RedButtonManager()  {
            Tempo::AddListener(this);
        }

    public:
        enum State
        {
            WaitForPresses,
            SyncToMeasure,
            PlayAnimation
        };
        State state = WaitForPresses;

        static RedButtonManager &getInstance()
        {
            static RedButtonManager instance;
            return instance;
        }

        void setRedButtonPressed(int button, bool pressed)
        {
            if (button < 0 || button >= 6)
            {
                Log::error("RedButtonManager", "Button index out of range: %d", button);
                return;
            }
            this->pressed[button] = pressed;

            if (amountPressed() == 6 && state == WaitForPresses)
            {
                alignmentStart = Tempo::GetProgress(alignTo);
                state = SyncToMeasure;
            }

            if (amountPressed() == 0 && state == PlayAnimation)
            // if (amountPressed() < 6 && state == PlayAnimation)
            {
                state = WaitForPresses;
            }
        }

        bool isRedButtonPressed(int button) const
        {
            if (button < 0 || button >= 6)
            {
                Log::error("RedButtonManager", "Button index out of range: %d", button);
                return false;
            }
            return pressed[button];
        }

        int amountPressed() const
        {
            return std::count(std::begin(pressed), std::end(pressed), true);
        }

        void OnBeat(int beatNr, const char *sourceName) override
        {
            if (beatNr % alignTo != 0)
                return; // Only reset on every 4th beat

            if (state != SyncToMeasure)
                return; // Only reset if we are in sync mode

            state = PlayAnimation;
        }

        float getSyncProgress()
        {
            return Utils::rescale_c(
                Tempo::GetProgress(alignTo),
                0, 1, alignmentStart, 1);
        }
    };

    class ButtonPressedPattern : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            500, Transition::none, 0);
        LFO<Glow> lfo = LFO<Glow>(500);
        std::vector<float> radii;
        int buttonIndex;
        RedButtonManager &redButtonManager = RedButtonManager::getInstance();
        char nameBuf[50];
        float fadeFactors[7] = {0, 0.15, 0.3, 0.5, 0.5, 0.5, 0.5};

        // BeatWatcher watcher = BeatWatcher();

    public:
        ButtonPressedPattern(PixelMap3d *map, int buttonIndex)
        {
            float r = 2360. / 4100;
            float xc = r * cos(float(buttonIndex) / 6 * 2 * M_PI);
            float yc = 0.15;
            float zc = r * sin(float(buttonIndex) / 6 * 2 * M_PI);
            std::transform(map->begin(), map->end(), std::back_inserter(radii), [xc, yc, zc](PixelPosition3d pos) -> float
                           { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2) + pow(pos.z - zc, 2)); });

            this->buttonIndex = buttonIndex;

            snprintf(nameBuf, sizeof(nameBuf), "Button feedback %d", buttonIndex + 1);
            this->name = nameBuf;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            redButtonManager.setRedButtonPressed(buttonIndex, active);
            if (!transition.Calculate(active))
                return;

            if (redButtonManager.state != RedButtonManager::WaitForPresses)
                return;

            int amountPressed = redButtonManager.amountPressed();

            float fadeFactor = fadeFactors[amountPressed];

            for (int i = 0; i < width; i++)
            {
                RGBA color = params->getPrimaryColour();
                float distanceFade = Utils::rescale_c(radii[i], 1, 0, 0, fadeFactor);

                float lfoValue = lfo.getValue(radii[i] * 20);
                pixels[i] += color * lfoValue * distanceFade * transition.getValue();
            }
        }
    };

    class SyncToMeasurePattern : public Pattern<RGBA>
    {
        RedButtonManager &redButtonManager = RedButtonManager::getInstance();
        PixelMap3d::Spherical *map ;
        FadeDown fade = FadeDown(200);


    public:
        SyncToMeasurePattern(PixelMap3d::Spherical *map)
        {
            this->map = map;
            this->name = "Sync to measure";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            if (redButtonManager.state != RedButtonManager::SyncToMeasure)
                return;

            for (int i = 0; i < width; i++)
            {
                float fadePos = Utils::rescale(map->th(i),0,1, 0.45*M_PI,0);
                float syncPos = redButtonManager.getSyncProgress();
                float fadeValue = abs( syncPos - fadePos) < 0.025 ? 1 : 0;
                
                pixels[i] = params->getSecondaryColour() * fadeValue;
            }
        }
    };

    template <class T_COLOUR>
    class TimedAnimate : public Pattern<T_COLOUR>
    {
        Timeline timeline;
        int onDuration;
        int onStart;
        RedButtonManager &redButtonManager = RedButtonManager::getInstance();
        Pattern<T_COLOUR> *innerpattern;

    public:
        TimedAnimate(Pattern<T_COLOUR> *innerpattern, int onDuration, int onstart=0)
        {
            this->innerpattern = innerpattern;
            this->onDuration = onDuration;
            this->onStart = onstart;

            this->name = innerpattern->name;
        }


        inline void Calculate(T_COLOUR *pixels, int width, bool active, Params *params) override
        {
            if (redButtonManager.state == RedButtonManager::SyncToMeasure){
                timeline.reset();
            }

            timeline.FrameStart();

            bool active2 = 
                active && 
                timeline.GetTimelinePosition() > 0 &&
                timeline.GetTimelinePosition() >= onStart &&
                timeline.GetTimelinePosition() < onStart + onDuration;

            innerpattern->Calculate(pixels, width, active2, params);
        }
    };

}