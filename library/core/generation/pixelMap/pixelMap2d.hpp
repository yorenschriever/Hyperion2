#pragma once

#include <algorithm>
#include <inttypes.h>
#include <math.h>
#include <memory>
#include <vector>

using namespace std;

class PixelMap;
class PixelMapPolar;

enum class AngularUnits
{
    DEG,
    RAD
};

float toRadians(float angle, AngularUnits units)
{
    if (units == AngularUnits::DEG)
        return angle * (M_PI / 180.0);
    return angle;
}


struct PixelPosition
{
    float x;
    float y;
};

struct PolarPixelPosition
{
    float r;
    float th;
};

using PixelMapPtr = std::shared_ptr<const PixelMap>;

class PixelMap : public vector<PixelPosition>
{
public:
    using vector<PixelPosition>::vector;

    float x(unsigned int index) const
    {
        return this->operator[](index).x;
    }

    float y(unsigned int index) const
    {
        return this->operator[](index).y;
    }

    PixelMapPtr getPtr() const
    {
        return std::make_shared<PixelMap>(*this);
    }

    class Polar;
    using PolarPtr = std::shared_ptr<const PixelMap::Polar>;

    class Polar : public vector<PolarPixelPosition>
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

        PolarPtr rotate(float angle, AngularUnits units = AngularUnits::DEG) const
        {
            angle = toRadians(angle, units);
            Polar rotated;
            transform(
                this->begin(),
                this->end(),
                back_inserter(rotated), [angle](PolarPixelPosition pos) -> PolarPixelPosition
                { return {
                      .r = pos.r,
                      .th = pos.th + angle}; });
            return std::make_shared<PixelMap::Polar>(rotated);
        }
    };

    PolarPtr toPolar() const
    {
        PixelMap::Polar polar;
        transform(
            this->begin(),
            this->end(),
            back_inserter(polar), [](PixelPosition pos) -> PolarPixelPosition
            { 
                float x = pos.x;
                float y = pos.y;
                return {
                    .r = sqrt(y * y + x * x),
                    .th = atan2(y, x)
                }; });
        return std::make_shared<PixelMap::Polar>(polar);
    }
};
