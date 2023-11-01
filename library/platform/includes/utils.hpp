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