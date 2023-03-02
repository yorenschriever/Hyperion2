#pragma once

#include <inttypes.h>

//You should derive from this class when writing your own output
class Output
{

public:
    //The pipe will call this function often to tell you the length of the 
    //data you are going to receive. Implement this function to allocate enough 
    //memory. Be sure to check if the length has actually changed before 
    //starting the allocation process
    virtual void setLength(int len) = 0;

    //The pipe will call this function if it has new data. You have to store 
    //the data in your buffer. Size and index are in bytes. (Outputs do not know
    //about pixels or colours)
    //The pipe can pass the data by calling this function for each pixel individually
    //or by using one call to send all data at once. This is decided by the pipe, 
    //your implementation should be able to handle both
    virtual void setData(uint8_t *data, int size, int index) = 0;

    //After all data is set, the pipe calls show() as a sign for you to 
    //send out the data. This should be done asynchronously, and this function should 
    //return as fast as possible.
    virtual void show() = 0;

    //After all pipes are processed, postProcess is called. You can use this to process
    //grouped behaviour, for example combining dmx outputs and send out a dmx frame.
    //postProcess will always be called on each iteration. Show() is only called if the
    //corresponding input had any data.
    virtual void postProcess() {};

    //Implement this to initialize your output. Do not put initialization code in the
    //constructor.
    virtual void begin() = 0;

    //Set all data to 0
    virtual void clear() = 0; //TODO make template for color and set buffer to value of that colour
    
    //The pipe will use this function to check if you transmission process is finished.
    //If not it will not send new data
    virtual bool ready() = 0;
};
