
#pragma once

#include "log.hpp"
#include <stdint.h>
// Permute can be used to get a list of numbers in random order
class Permute
{

private:
    int size=-1;

public:
    int *at = 0;

    Permute(int size = 0)
    {
        setSize(size);
    }

    void setSize(int size)
    {
        if (size == this->size)
            return;
        at = (int*) realloc(at, size * sizeof(int));
        this->size = size;

        order();
        permute();
    }

    void permute()
    {
        if (size <= 1)
            return;
        unsigned int i;
        for (i = 0; i <= size - 2; i++)
        {
            unsigned int j = i + Utils::random(0, size - i); /* A random integer such that i ≤ j < n */

            // On linux i have strange bug where sometimes at == nullptr.
            // It seems to occur when you set the size in the constructor,
            // and later call permute.
            // For now this check prevents it form crashing, but it does not
            // solve the undelying issue.
            if (at == nullptr)
            {
                Log::error("PERMUTE", "at = null, size=%d. Make sure to also check for null in your pattern before accessing permute.at", size);
                return;
            }

            /* Swap the randomly picked element with permutation[i] */
            int temp = at[i];
            at[i] = at[j];
            at[j] = temp;
        }
    }

    void order()
    {
        for (int i = 0; i < size; i++)
            at[i] = i;
    }
};