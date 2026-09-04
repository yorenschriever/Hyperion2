#pragma once

#include <algorithm>
#include <inttypes.h>
#include <math.h>
#include <memory>
#include <vector>

#include "pixelMap.hpp"

using namespace std;

class PixelMap3d;
class PixelMap3dCylindrical;
class PixelMap3dSpherical;

struct PixelPosition3d
{
    float x;
    float y;
    float z;
};

struct CylindricalPixelPosition
{
    float r;
    float th;
    float z;
};

struct SphericalPixelPosition
{
    float r;
    float th;  // angle from top
    float phi; // angle from x-axis in xy-plane
};

using PixelMap3dPtr = std::shared_ptr<const PixelMap3d>;

class PixelMap3d : public vector<PixelPosition3d>
{
public:
    using vector<PixelPosition3d>::vector;

    float x(unsigned int index) const
    {
        return this->operator[](index).x;
    }

    float y(unsigned int index) const
    {
        return this->operator[](index).y;
    }

    float z(unsigned int index) const
    {
        return this->operator[](index).z;
    }

    PixelMap3dPtr getPtr() const
    {
        return std::make_shared<PixelMap3d>(*this);
    }

    class Cylindrical;
    using CylindricalPtr = std::shared_ptr<const PixelMap3d::Cylindrical>;

    class Cylindrical : public vector<CylindricalPixelPosition>
    {
    public:
        float r(unsigned int index) const
        {
            return this->operator[](index).r;
        }

        float th(unsigned int index) const
        {
            return this->operator[](index).th;
        }

        float z(unsigned int index) const
        {
            return this->operator[](index).z;
        }

        CylindricalPtr rotate(float angle, AngularUnits units = AngularUnits::DEG) const
        {
            angle = toRadians(angle, units);
            Cylindrical rotated;
            transform(
                this->begin(),
                this->end(),
                back_inserter(rotated), [angle](CylindricalPixelPosition pos) -> CylindricalPixelPosition
                { return {
                      .r = pos.r,
                      .th = pos.th + angle,
                      .z = pos.z}; });
            return std::make_shared<PixelMap3d::Cylindrical>(rotated);
        }
    };

    // to Cylindrical coordinates where th==0 points to the top instead of to the right
    CylindricalPtr toCylindricalXZ() const
    {
        Cylindrical cylindricalXZ;
        transform(
            this->begin(),
            this->end(),
            back_inserter(cylindricalXZ), [](PixelPosition3d pos) -> CylindricalPixelPosition
            { 
                float x = pos.x;
                float z = pos.z;
                return {
                    .r = sqrt(z * z + x * x),
                    .th = atan2(x, -1*z),
                    .z = pos.y
                }; });
        return std::make_shared<PixelMap3d::Cylindrical>(cylindricalXZ);
    }

    CylindricalPtr toCylindricalYZ() const
    {
        Cylindrical cylindricalYZ;
        transform(
            this->begin(),
            this->end(),
            back_inserter(cylindricalYZ), [](PixelPosition3d pos) -> CylindricalPixelPosition
            { 
                float y = pos.y;
                float z = pos.z;
                return {
                    .r = sqrt(z * z + y * y),
                    .th = atan2(y, -1*z),
                    .z = pos.x
                }; });
        return std::make_shared<PixelMap3d::Cylindrical>(cylindricalYZ);
    }

    const CylindricalPtr toCylindricalXY() const
    {
        Cylindrical cylindricalXY;
        transform(
            this->begin(),
            this->end(),
            back_inserter(cylindricalXY), [](PixelPosition3d pos) -> CylindricalPixelPosition
            { 
                float x = pos.x;
                float y = pos.y;
                return {
                    .r = sqrt(y * y + x * x),
                    .th = atan2(y, x),
                    .z = pos.z
                }; });
        return std::make_shared<PixelMap3d::Cylindrical>(cylindricalXY);
    }

    class Spherical;
    using SphericalPtr = std::shared_ptr<const PixelMap3d::Spherical>;

    class Spherical : public vector<SphericalPixelPosition>
    {
    public:
        float r(unsigned int index) const
        {
            return this->operator[](index).r;
        }

        float th(unsigned int index) const
        {
            return this->operator[](index).th;
        }

        float phi(unsigned int index) const
        {
            return this->operator[](index).phi;
        }
    };

    SphericalPtr toSphericalXZ() const
    {
        Spherical sphericalXZ;

        transform(
            this->begin(),
            this->end(),
            back_inserter(sphericalXZ), [](PixelPosition3d pos) -> SphericalPixelPosition
            { 
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
                }; });
        return std::make_shared<PixelMap3d::Spherical>(sphericalXZ);
    }

    PixelMapPtr toTopView() const
    {
        PixelMap topView;
        transform(
            this->begin(),
            this->end(),
            back_inserter(topView), [](PixelPosition3d pos) -> PixelPosition
            { return {
                  .x = pos.x,
                  .y = pos.y}; });
        return std::make_shared<PixelMap>(topView);
    }

    PixelMapPtr toFrontView() const
    {
        PixelMap frontView;
        transform(
            this->begin(),
            this->end(),
            back_inserter(frontView), [](PixelPosition3d pos) -> PixelPosition
            { return {
                  .x = pos.x,
                  .y = pos.z}; });
        return std::make_shared<PixelMap>(frontView);
    }

    PixelMapPtr toSideView() const
    {
        PixelMap sideView;
        transform(
            this->begin(),
            this->end(),
            back_inserter(sideView), [](PixelPosition3d pos) -> PixelPosition
            { return {
                  .x = pos.y,
                  .y = pos.z}; });
        return std::make_shared<PixelMap>(sideView);
    }
};