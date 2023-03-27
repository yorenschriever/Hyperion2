#pragma once

#include <inttypes.h>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

struct PixelPosition
{
    float x;
    float y;
};

class PixelMap: public vector<PixelPosition>  {

public:

    struct PolarPixelPosition
    {
        float r;
        float th;
    };

    typedef vector<PolarPixelPosition> Polar;

    using vector<PixelPosition>::vector;

private:
    Polar polar;
    Polar polar90;

public:

    Polar toPolar()
    {
        if (polar.size() == this->size())
            return polar;

        transform(
            this->begin(), 
            this->end(), 
            back_inserter(polar), [](PixelPosition pos) -> PolarPixelPosition{ 
                return {
                    .r = sqrt(pos.y * pos.y + pos.x * pos.x),
                    .th = atan2(pos.y, pos.x)
                };
            });
        return polar;
    }

    //to polar coordinates where th==0 points to the top instead of to the right
    Polar toPolarRotate90()
    {
        if (polar90.size() == this->size())
            return polar90;

        transform(
            this->begin(), 
            this->end(), 
            back_inserter(polar90), [](PixelPosition pos) -> PolarPixelPosition{ 
                return {
                    .r = sqrt(pos.y * pos.y + pos.x * pos.x),
                    .th = atan2(pos.x, -1*pos.y)
                };
            });
        return polar90;
    }
};



struct PixelPosition3d
{
    float x;
    float y;
    float z;
};

//typedef std::vector<PixelPosition3d> PixelMap3d;

class PixelMap3d: public vector<PixelPosition3d>  {

public:

    struct CylindricalPixelPosition
    {
        float r;
        float th;
        float z;
    };

    typedef vector<CylindricalPixelPosition> Cylindrical;

    using vector<PixelPosition3d>::vector;

private:
    Cylindrical cylindrical;
    Cylindrical cylindrical90;

public:

    Cylindrical toCylindrical()
    {
        if (cylindrical.size() == this->size())
            return cylindrical;

        transform(
            this->begin(), 
            this->end(), 
            back_inserter(cylindrical), [](PixelPosition3d pos) -> CylindricalPixelPosition{ 
                return {
                    .r = sqrt(pos.y * pos.y + pos.x * pos.x),
                    .th = atan2(pos.y, pos.x),
                    .z = pos.z
                };
            });
        return cylindrical;
    }

    //to Cylindrical coordinates where th==0 points to the top instead of to the right
    Cylindrical toCylindricalRotate90()
    {
        if (cylindrical90.size() == this->size())
            return cylindrical90;

        transform(
            this->begin(), 
            this->end(), 
            back_inserter(cylindrical90), [](PixelPosition3d pos) -> CylindricalPixelPosition{ 
                return {
                    // .r = sqrt(pos.y * pos.y + pos.x * pos.x),
                    // .th = atan2(pos.x, -1*pos.y),
                    // .z = pos.z
                    .r = sqrt(pos.z * pos.z + pos.x * pos.x),
                    .th = atan2(pos.x, -1*pos.z),
                    .z = pos.y
                };
            });
        return cylindrical90;
    }
};