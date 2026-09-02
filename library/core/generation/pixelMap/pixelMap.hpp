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

    PixelMap(const PixelMap& other)
    {
        //copy the elements, but not the derived structures like polar or polar90
        this->assign(other.begin(), other.end());
    }

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

    Polar polar;
    Polar *toPolar()
    {
        if (polar.size() == this->size())
            return &polar;

        polar.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(polar), [](PixelPosition pos) -> PolarPixelPosition{ 
                float x = pos.x;
                float y = pos.y;
                return {
                    .r = sqrt(y * y + x * x),
                    .th = atan2(y, x)
                };
            });
        return &polar;
    }

    //to polar coordinates where th==0 points to the top instead of to the right
    Polar polar90;
    Polar *toPolarRotate90()
    {
        if (polar90.size() == this->size())
            return &polar90;

        polar90.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(polar90), [](PixelPosition pos) -> PolarPixelPosition{ 
                float x = pos.x;
                float y = pos.y;
                return {
                    .r = sqrt(y * y + x * x),
                    .th = atan2(x, -1*y)
                };
            });
        return &polar90;
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

    struct SphericalPixelPosition
    {
        float r;
        float th; //angle from top
        float phi; //angle from x-axis in xy-plane
    };

    PixelMap3d(const PixelMap3d& other)
    {
        //copy the elements, but not the derived structures like cylindricalXY or spherical
        this->assign(other.begin(), other.end());
    }

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

        Cylindrical rotate(float angle)
        {
            Cylindrical rotated;

            transform(
                this->begin(), 
                this->end(), 
                back_inserter(rotated), [angle](CylindricalPixelPosition pos) -> CylindricalPixelPosition{ 
                    return {
                        .r = pos.r,
                        .th = pos.th + angle, 
                        .z = pos.z
                    };
                });
            return rotated;
        }
    };

    class Spherical: public vector<SphericalPixelPosition>
    {
        public:
        float r(unsigned int index){
            return this->operator[](index).r;
        }

        float th(unsigned int index){
            return this->operator[](index).th;
        }

        float phi(unsigned int index){
            return this->operator[](index).phi;
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

    Cylindrical cylindricalXY;
    Cylindrical *toCylindricalXY()
    {
        if (cylindricalXY.size() == this->size())
            return &cylindricalXY;

        cylindricalXY.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(cylindricalXY), [](PixelPosition3d pos) -> CylindricalPixelPosition{ 
                float x = pos.x;
                float y = pos.y;
                return {
                    .r = sqrt(y * y + x * x),
                    .th = atan2(y, x),
                    .z = pos.z
                };
            });
        return &cylindricalXY;
    }

    //to Cylindrical coordinates where th==0 points to the top instead of to the right
    Cylindrical cylindricalXZ;
    Cylindrical *toCylindricalXZ()
    {
        if (cylindricalXZ.size() == this->size())
            return &cylindricalXZ;

        cylindricalXZ.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(cylindricalXZ), [](PixelPosition3d pos) -> CylindricalPixelPosition{ 
                float x = pos.x;
                float z = pos.z;
                return {
                    .r = sqrt(z * z + x * x),
                    .th = atan2(x, -1*z),
                    .z = pos.y
                };
            });
        return &cylindricalXZ;
    }


    Cylindrical cylindricalYZ;
    Cylindrical *toCylindricalYZ()
    {
        if (cylindricalYZ.size() == this->size())
            return &cylindricalYZ;

        cylindricalYZ.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(cylindricalYZ), [](PixelPosition3d pos) -> CylindricalPixelPosition{ 
                float y = pos.y;
                float z = pos.z;
                return {
                    .r = sqrt(z * z + y * y),
                    .th = atan2(y, -1*z),
                    .z = pos.x
                };
            });
        return &cylindricalYZ;
    }


    Spherical sphericalXZ;
    Spherical *toSphericalXZ()
    {
        if (sphericalXZ.size() == this->size())
            return &sphericalXZ;

        sphericalXZ.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(sphericalXZ), [](PixelPosition3d pos) -> SphericalPixelPosition{ 
                float x = pos.x;
                float y = pos.y;
                float z = pos.z;
                float r = sqrt(x * x + y * y + z * z);
                if (r == 0) // avoid division by zero
                    return { .r = 0, .th = 0, .phi = 0 };

                float th = acos(y / r); // angle from the top
                float phi = atan2(z, x); // angle in the xy-plane from the x-axis
                return {
                    .r = r,
                    .th = th,
                    .phi = phi
                };
            });
        return &sphericalXZ;
    }

    PixelMap topView;
    PixelMap *toTopView()
    {
        if (topView.size() == this->size())
            return &topView;

        topView.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(topView), [](PixelPosition3d pos) -> PixelPosition{ 
                return {
                    .x = pos.x,
                    .y = pos.y
                };
            });
        return &topView;
    }

    PixelMap frontView;
    PixelMap *toFrontView()
    {
        if (frontView.size() == this->size())
            return &frontView;

        frontView.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(frontView), [](PixelPosition3d pos) -> PixelPosition{ 
                return {
                    .x = pos.x,
                    .y = pos.z
                };
            });
        return &frontView;
    }

    PixelMap sideView;
    PixelMap *toSideView()
    {
        if (sideView.size() == this->size())
            return &sideView;

        sideView.clear();
        transform(
            this->begin(), 
            this->end(), 
            back_inserter(sideView), [](PixelPosition3d pos) -> PixelPosition{ 
                return {
                    .x = pos.y,
                    .y = pos.z
                };
            });
        return &sideView;
    }

};