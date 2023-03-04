
#pragma once

#include "utils.hpp"
#include <stdint.h>

template <class T_INTERVAL_TYPE>
class Interval
{

public:
    //create a stochastic interval. the provided average is in ms
    Interval(int averageInterval = 1000)
    {
        this->averageInterval = averageInterval;
        this->lastTime = Utils::millis();
    }

    void setAverageInterval(int averageInterval)
    {
        this->averageInterval = averageInterval;
    }

    bool Happened()
    {
        unsigned long now = Utils::millis();
        if (now - lastTime < currentDuration) //this method should handle the millis overflow correctly
            return false;

        lastTime = now;
        currentDuration = T_INTERVAL_TYPE::getRandomDuration(averageInterval);
        return true;
    }

protected:
    unsigned long lastTime;
    int currentDuration;
    int averageInterval;
};

class ConstantInterval
{
public:
    static int getRandomDuration(int averageInterval)
    {
        return averageInterval;
    }
};

class RandomInterval
{
public:
    static int getRandomDuration(int averageInterval)
    {
        return Utils::random(0, 2 * averageInterval);
    }
};

class PoissonInterval
{
public:
    static int getRandomDuration(int averageInterval)
    {
        float uniformRandom = Utils::random_f();
        float randomTime = -1 * log(1. - uniformRandom);
        return randomTime * averageInterval;
    }
};