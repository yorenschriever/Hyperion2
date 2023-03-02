#pragma once

class Utils
{
public:
    static unsigned long millis();
    static unsigned long micros();

    static void exit();

    static int random(int minimum, int maximum);

    static int get_free_heap();

    static int constrain(int value, int minimun, int maximum){
        if (value < minimun) return minimun;
        if (value > maximum) return maximum;
        return value;
    }
};