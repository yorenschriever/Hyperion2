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

    class Polar: public vector<PolarPixelPosition>
    {
        public:
        float r(unsigned int index){
            return this->operator[](index).r;
        }

        float th(unsigned int index){
            return this->operator[](index).th;
        }
    };

    using vector<PixelPosition>::vector;

    float x(unsigned int index){
        return this->operator[](index).x;
    }

    float y(unsigned int index){
        return this->operator[](index).y;
    }

    Polar toPolar()
    {
        Polar polar;

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
        Polar polar90;

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

class PixelMap3d: public vector<PixelPosition3d>  {

public:

    struct CylindricalPixelPosition
    {
        float r;
        float th;
        float z;
    };

    class Cylindrical: public vector<CylindricalPixelPosition>
    {
        public:
        float r(unsigned int index){
            return this->operator[](index).r;
        }

        float th(unsigned int index){
            return this->operator[](index).th;
        }

        float z(unsigned int index){
            return this->operator[](index).z;
        }
    };

    using vector<PixelPosition3d>::vector;

    float x(unsigned int index){
        return this->operator[](index).x;
    }

    float y(unsigned int index){
        return this->operator[](index).y;
    }

    float z(unsigned int index){
        return this->operator[](index).z;
    }

    Cylindrical toCylindricalXY(float centerX=0, float centerY=0)
    {
        Cylindrical cylindricalXY;

        transform(
            this->begin(), 
            this->end(), 
            back_inserter(cylindricalXY), [centerX,centerY](PixelPosition3d pos) -> CylindricalPixelPosition{ 
                float x = pos.y - centerX;
                float y = pos.y - centerY;
                return {
                    .r = sqrt(y * y + x * x),
                    .th = atan2(y, x),
                    .z = pos.z
                };
            });
        return cylindricalXY;
    }

    //to Cylindrical coordinates where th==0 points to the top instead of to the right
    Cylindrical toCylindricalXZ(float centerX=0, float centerZ=0)
    {
        Cylindrical cylindricalXZ;

        transform(
            this->begin(), 
            this->end(), 
            back_inserter(cylindricalXZ), [centerX, centerZ](PixelPosition3d pos) -> CylindricalPixelPosition{ 
                float x = pos.x - centerX;
                float z = pos.z - centerZ;
                return {
                    .r = sqrt(z * z + x * x),
                    .th = atan2(x, -1*z),
                    .z = pos.y
                };
            });
        return cylindricalXZ;
    }
};