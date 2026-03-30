#pragma once
#include <limits.h>

class Utils
{
public:
    static unsigned long millis();
    static unsigned long micros();

    static void exit();

    static int random(int minimum, int maximum);
    static float random_f();
    static void random_seed();

    static int get_free_heap();

    static int constrain(int value, int minimum, int maximum)
    {
        if (value < minimum)
            return minimum;
        if (value > maximum)
            return maximum;
        return value;
    }

    static float constrain_f(float value, float minimum, float maximum)
    {
        if (value < minimum)
            return minimum;
        if (value > maximum)
            return maximum;
        return value;
    }

    // void testModulus()
    // {
    //     Log::info("","mod 50   [0,100]     = 50: %d",Utils::mod(50,0,100));
    //     Log::info("","mod 0    [0,100]     = 0: %d",Utils::mod(0,0,100));
    //     Log::info("","mod 100  [0,100]     = 0: %d",Utils::mod(100,0,100));
    //     Log::info("","mod 110  [0,100]     = 10: %d",Utils::mod(110,0,100));
    //     Log::info("","mod 210  [0,100]     = 10: %d",Utils::mod(210,0,100));
    //     Log::info("","mod -10  [0,100]     = 90: %d",Utils::mod(-10,0,100));
    //     Log::info("","mod 210  [100,200]   = 110: %d",Utils::mod(210,100,200));
    //     Log::info("","mod 310  [100,200]   = 110: %d",Utils::mod(310,100,200));
    //     Log::info("","mod 110  [-200,-100] = -190: %d",Utils::mod(110,-200,-100));
    //     Log::info("","mod -110 [-200,-100] = -110: %d",Utils::mod(-110,-200,-100));
    //     Log::info("","mod -210 [-200,-100] = -110: %d",Utils::mod(-210,-200,-100));

        // Log::info("","mod 0.5   [0,1]     = 0.5:  %0.1f",Utils::modf(0.5,0.,1.));
        // Log::info("","mod 0     [0,1]     = 0:    %0.1f",Utils::modf(0,0.,1.));
        // Log::info("","mod 1     [0,1]     = 0:    %0.1f",Utils::modf(1,0.,1.));
        // Log::info("","mod 1.1   [0,1]     = 0.1:  %0.1f",Utils::modf(1.1,0.,1.));
        // Log::info("","mod 2.1   [0,1]     = 0.1:  %0.1f",Utils::modf(2.1,0.,1.));
        // Log::info("","mod -0.1  [0,1]     = 0.9:  %0.1f",Utils::modf(-0.1,0.,1.));
        // Log::info("","mod 2.1   [1,2]     = 1.1:  %0.1f",Utils::modf(2.1,1.,2.));
        // Log::info("","mod 3.1   [1,2]     = 1.1:  %0.1f",Utils::modf(3.1,1.,2.));
        // Log::info("","mod 1.1   [-2,-1]   = -1.9: %0.1f",Utils::modf(1.1,-2.,-1.));
        // Log::info("","mod -1.1  [-2,-1]   = -1.1: %0.1f",Utils::modf(-1.1,-2.,-1.));
        // Log::info("","mod -2.1  [-2,-1]   = -1.1: %0.1f",Utils::modf(-2.1,-2.,-1.));
    // }

    //this maps the value within the given range. 
    //What is also nice abut this function is that it doesn't flip for negative values.
    //(normal modulo sometimes works with the absolute value of the input)
    static int mod(int value, int minimum=0, int maximum=100)
    {
        if (value < minimum){
            int wrappedCount = (minimum - value) / (maximum - minimum) + 1;
            return value + wrappedCount * (maximum - minimum);
        }
        if (value >= maximum){
            int wrappedCount = (value - maximum) / (maximum - minimum) + 1;
            return value - wrappedCount * (maximum - minimum);
        }
        return value;
    }

    static float modf(float value, float minimum=0, float maximum=1){
        return modulus_f(value, minimum, maximum);
    }

    static float modulus_f(float value, float minimum=0, float maximum=1)
    {
        float range = maximum - minimum;
        if (value < minimum){
            int wrappedCount = (minimum - value) / range + 1;
            return value + wrappedCount * range;
        }
        if (value >= maximum){
            int wrappedCount = (value - maximum) / range + 1;
            return value - wrappedCount * range;
        }
        return value;
    }

    static float rescale(float value, float minimum_out, float maximum_out, float minimum_in = 0, float maximum_in = 1)
    {
        float normalized = (value - minimum_in) / (maximum_in - minimum_in);
        // normalized = constrain_f(normalized, 0, 1);
        return minimum_out + normalized * (maximum_out - minimum_out);
    }

    static float rescale_c(float value, float minimum_out, float maximum_out, float minimum_in = 0, float maximum_in = 1)
    {
        float normalized = (value - minimum_in) / (maximum_in - minimum_in);
        normalized = constrain_f(normalized, 0, 1);
        return minimum_out + normalized * (maximum_out - minimum_out);
    }

    static void *malloc_dma(int size);
    static void *realloc_dma(void *ptr, int size);

    static int getRuntimeSessionId()
    {
        if (runtimeSessionId == 0)
        {
            runtimeSessionId = Utils::random(0, INT_MAX);
        }
        return runtimeSessionId;
    }

private:
    static int runtimeSessionId;
};