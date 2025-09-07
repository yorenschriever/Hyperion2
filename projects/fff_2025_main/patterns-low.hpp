#pragma once

float fromTop2(float y){

    return Utils::rescale(y,0,1,-0.5,.5);
}

float fromBottom2(float y){
    return 1.0 - fromTop2(y);
}


namespace Low
{

    class StaticGradientPattern : public Pattern<RGBA>
    {
        PixelMap3d *map;
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        StaticGradientPattern(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Static gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            float height = params->getSize(0.2,1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                float h = fromBottom(map->y(index))* height;
                RGBA colour = params->getGradient(h * 255);
                pixels[index] = colour * h * transition.getValue();
            }
        }
    };

    class OnBeatColumnChaseUpPattern : public Pattern<RGBA>
    {
        Transition transition;
        FadeDown fade[6] = {
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200),
            FadeDown(200)};
        BeatWatcher watcher = BeatWatcher();
        PixelMap3d::Spherical *map;
        int pos = 0;

    public:
        OnBeatColumnChaseUpPattern(PixelMap3d::Spherical *map)
        {
            this->map = map;
            this->name = "On beat column chase up";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            if (watcher.Triggered())
            {
                pos = (pos + 1) % 6;
                fade[pos].reset();
            }

            float velocity = params->getVelocity(2000, 100);
            float tailSize = params->getSize(300,50);

            for (int c=0;c<6; c++)
            {
                fade[c].setDuration(tailSize);
            }

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                int column = map->phi(index) * 6 / (2. * M_PI);
                float columnCenter = (float)column * 2 * M_PI / 6;

                if (abs(map->phi(index) - columnCenter) > 0.01)
                    continue; 

                column = (column + 6) % 6;

                float normalized_angle = Utils::rescale_c(map->th(index), 1, 0, 0, 0.6 * M_PI);
                float fadePosition = fade[column].getValue(normalized_angle * velocity);

                RGBA color = params->getPrimaryColour(); 
                pixels[index] = color * fadePosition * transition.getValue();
                
                // float y = fromTop2(map->y(index));
                // float fadePosition = fade[column].getValue(y * velocity);
                // RGBA color = params->getPrimaryColour(); 
                // pixels[index] = color * fadePosition * (1 - y) * transition.getValue();
            }

            // for (int column = 0; column < 6; column++)
            // {
            //     int columnStart = column * width / 6;
            //     int columnEnd = columnStart + width / 6;

            //     fade[column].duration = tailSize;

            //     for (int i = columnStart; i < columnEnd; i++)
            //     {
            //         float y = fromTop(map->y(i));
            //         float fadePosition = fade[column].getValue(y * velocity);
            //         RGBA color = params->getPrimaryColour(); 
            //         pixels[i] = color * fadePosition * (1 - y) * transition.getValue();
            //     }
            // }
        }
    };

    class HorizontalSin : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFOTempo<Glow> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        HorizontalSin(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Horizontal sin";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            // lfo.setPeriodExponential(params->getVelocity(3, 1.01));
            float velo = params->getVelocity();
            int period = 4;
            if (velo<0.33) period = 8;
            if ( velo > 0.66 ) period = 1;

            lfo.setPeriod(6*period);
            lfo.setDutyCycle(params->getSize(0.03,0.5));

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                // RGBA color = params->getPrimaryColour(); 
                RGBA color = params->getGradient(fromTop(map->z(index))*255); 
                pixels[index] = color * lfo.getValue(around(map->th(index))) * transition.getValue();
                // pixels[index] = color * lfo.getValue(fromTop(map->z(index))) * transition.getValue();
            }
        }

    };

    class HorizontalSaw : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SawDown> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        HorizontalSaw(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Horzontal saw";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            lfo.setDutyCycle(params->getSize(0.06,1));
            bool orientationHorizontal = params->getVariant() > 0.5;

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                RGBA color = params->getPrimaryColour(); 
                float lfoArg = orientationHorizontal ? around(map->th(index)) : fromTop(map->z(index));
                pixels[index] = color * lfo.getValue(lfoArg) * fromBottom(map->z(index)) * transition.getValue();
            }
        }
    };

    class GrowShrink : public Pattern<RGBA>
    {
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);
        LFO<SinFast> lfo;
        PixelMap3d::Cylindrical *map;

    public:
        GrowShrink(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Grow shrink";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return;

            lfo.setPeriod(params->getVelocity(5000,500));
            //lfo.setPulseWidth(params->getSize(0.06,1));
            float size = params->getSize(0.1,0.5);
            float offset = params->getOffset(0,1);

            for (int index = 0; index < std::min(width, (int)map->size()); index++)
            {
                //RGBA color = params->getPrimaryColour(); 
                //float lfoArg = orientationHorizontal ? around(map->th(index)) : fromTop(map->z(index));
                
                int angle = around(map->th(index)) * 3600;
                if ((angle+300) % 600 > 0)
                    continue;

                float lfoSize = lfo.getValue(offset * around(map->th(index))) * size; 
                float distance = abs(map->z(index) + 0.07);
                if (distance > lfoSize)
                    continue;

                float distanceAsRatio = 1 - distance / lfoSize ;

                pixels[index] = params->getGradient(distanceAsRatio * 255) * distanceAsRatio * transition.getValue();
            }
        }
    };

    class GlowPulsePattern : public Pattern<RGBA>
    {
        Permute perm;
        LFO<Glow> lfo = LFO<Glow>(10000);
        Transition transition;
        PixelMap3d *map;

    public:
        GlowPulsePattern(PixelMap3d *map)
        {
            this->map = map;
            this->name = "Glow pulse";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            int density = width/481;
            lfo.setPeriod(params->getVelocity(10000,500));
            lfo.setDutyCycle(params->getAmount(0.05,0.5));
            float fadeSize = params->getSize(0,0.75)-0.5;
            perm.setSize(width);

            for (int index = 0; index < width; index++)
            {
                if (index % density != 0)
                   continue;
                float fade = Utils::constrain_f(fromBottom(map->y(perm.at[index]) - fadeSize),0,1);
                pixels[perm.at[index]] = params->getPrimaryColour() * fade * lfo.getValue(float(index)/width) * transition.getValue(index, width);
            }
        }
    };

 class VerticallyIsolated : public Pattern<RGBA>
    {
        Transition transition = Transition(
                200, Transition::fromStart, 1200,
                500, Transition::fromEnd, 1500);
        PixelMap3d::Cylindrical *map;

    public:
        VerticallyIsolated(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Vertically isolated";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            for (int index = 0; index < width; index++)
            {
                int angle = around(map->th(index)) * 3600;
                if ((angle+300) % 600 > 0)
                    continue;

                int z255 = fromBottom2(map->z(index)) * 255;
                pixels[index] = params->getGradient(z255)* transition.getValue(z255,255);
            }
        }
    };

     class RotatingRingsPattern : public Pattern<RGBA>
    {
        Transition transition;
        PixelMap3d::Cylindrical *map;
        LFO<Sin> ring1;
        LFO<Sin> ring2;

    public:
        RotatingRingsPattern(PixelMap3d::Cylindrical *map)
        {
            this->map = map;
            this->name = "Rotating rings";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params* params) override
        {
            if (!transition.Calculate(active))
                return; // the fade out is done. we can skip calculating pattern data

            auto col1 = params->getPrimaryColour() * transition.getValue();
            auto col2 = params->getSecondaryColour() * transition.getValue();
            float size = params->getSize(0.01,0.1);
            float zoffset = params->getVariant(0,-0.2);
            ring1.setPeriod(params->getVelocity(20000,2000));
            ring2.setPeriod(params->getVelocity(14000,1400));

            for (int index = 0; index < width; index++)
            {
                float z_norm = zoffset+2*fromTop2(map->z(index));
                float offset = around(map->th(index)) * params->getOffset();

                pixels[index] = col1 * softEdge(abs(z_norm - ring1.getValue(offset)), size);
                pixels[index] += col2 * softEdge(abs(z_norm - ring2.getValue(offset)), size);
            }
        }
    };

}
