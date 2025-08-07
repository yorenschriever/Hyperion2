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
        const int alignTo = 8; // Align to measure (4 beats)

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
        char nameBuf[18] = "Button circles  ";

        // BeatWatcher watcher = BeatWatcher();

    public:
        ButtonPressedPattern(PixelMap3d *map, int buttonIndex)
        {
            float r = 3360. / 4100;
            float xc = r * cos(float(buttonIndex) / 6 * 2 * M_PI);
            float yc = 0.15;
            float zc = r * sin(float(buttonIndex) / 6 * 2 * M_PI);
            std::transform(map->begin(), map->end(), std::back_inserter(radii), [xc, yc, zc](PixelPosition3d pos) -> float
                           { return sqrt(pow(pos.x - xc, 2) + pow(pos.y - yc, 2) + pow(pos.z - zc, 2)); });

            this->buttonIndex = buttonIndex;

            snprintf(nameBuf, sizeof(nameBuf), "Button circles %d", buttonIndex + 1);
            this->name = nameBuf;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            redButtonManager.setRedButtonPressed(buttonIndex, active);
            if (!transition.Calculate(active))
                return;

            if (redButtonManager.state != RedButtonManager::WaitForPresses)
                return;

            // if (watcher.Triggered() && Tempo::GetBeatNumber() % 4 == 0)
            // {
            //     pos = (pos + 1) % 6;
            //     fade[pos].reset();
            // }

            // float velocity = params->getVelocity(1000, 100);
            // for (int i = 0; i < 6; i++)
            //     fade[i].duration = params->getSize(400, 50) * velocity / 1000;

            // float density = 481./width;

            // std::vector<bool> pressed = {true, false, true, true, false, false};
            int amountPressed = redButtonManager.amountPressed();
            float fadeFactors[7] = {0, 0.15, 0.3, 0.5, 0.5, 0.5, 0.5};
            float fadeFactor = fadeFactors[amountPressed];

            for (int i = 0; i < width; i++)
            {
                // float fadePosition = fade[column].getValue(radii[column][i] * velocity);
                RGBA color = params->getPrimaryColour();
                // pixels[i] += color * fadePosition * transition.getValue();
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

    class FadingNoisePattern : public Pattern<RGBA>
    {
        RedButtonManager &redButtonManager = RedButtonManager::getInstance();
        Fade<Down, Cubic> fade = Fade<Down, Cubic>(600);
        Permute perm;

    public:
        FadingNoisePattern()
        {
            this->name = "Fading noise";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active || redButtonManager.state != RedButtonManager::PlayAnimation)
            {
                fade.reset();
                return;
            }

            perm.setSize(width);
            perm.permute();

            int noiseLevel = fade.getValue() * width / 3;
            for (int index = 0; index < noiseLevel; index++)
            {
                pixels[perm.at[index]] = params->getHighlightColour();
            }
        }
    };


    class StrobeFadePattern : public Pattern<RGBA>
    {
        RedButtonManager &redButtonManager = RedButtonManager::getInstance();
        //Fade<Down, Cubic> fade = Fade<Down, Cubic>(600);
        FadeDown fade = FadeDown(1500);
        PixelMap3d::Spherical *map;

    public:
        StrobeFadePattern(PixelMap3d::Spherical *map)
        {
            this->map = map;
            this->name = "Strobe fade noise";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active || redButtonManager.state != RedButtonManager::PlayAnimation)
            {
                fade.reset();
                return;
            }

            for (int index = 0; index < width; index++)
            {
                float fadePos = Utils::rescale(map->th(index), 0, 3000, 0, 2* M_PI);
                float fadeValue = Utils::rescale_c(fade.getValue(fadePos),0,1, 0, 0.5);
                RGBA col = Utils::millis() % 100 < 25 ? params->getHighlightColour() : RGBA(0,0,0,255);
                pixels[index] = col * fadeValue;
            }
        }
    };

}