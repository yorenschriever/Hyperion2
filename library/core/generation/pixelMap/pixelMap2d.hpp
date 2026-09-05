#pragma once

#include <algorithm>
#include <cfloat>
#include <inttypes.h>
#include <math.h>
#include <memory>
#include <vector>

using namespace std;

class PixelMap;
class PixelMapPolar;
class PixelMap3d;
using PixelMap3dPtr = std::shared_ptr<const PixelMap3d>;

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

    PixelMap3dPtr to3d() const;

    PixelMapPtr resizeAndTranslate(float scaleX, float scaleY, float x, float y) const
    {
        PixelMap result = *this;
        for (auto &pos : result)
        {
            pos.x = pos.x * scaleX + x;
            pos.y = pos.y * scaleY + y;
        }
        return std::make_shared<PixelMap>(result);
    }

    PixelMapPtr resize(float scale) const
    {
        return resizeAndTranslate(scale, scale, 0, 0);
    }

    PixelMapPtr resize(float scaleX, float scaleY) const
    {
        return resizeAndTranslate(scaleX, scaleY, 0, 0);
    }

    PixelMapPtr translate(float x, float y) const
    {
        return resizeAndTranslate(1, 1, x, y);
    }

    PixelMapPtr panelize(int panels_x, int panels_y, int pos_x, int pos_y) const
    {
        float scale = 1. / std::max(panels_x, panels_y);
        float start_x = -1 + scale + pos_x * scale * 2;
        float start_y = -1 + scale + pos_y * scale * 2;

        return resizeAndTranslate(scale, scale, start_x, start_y);
    }

    /*
    This rescales a map to fit in a -1 to 1 box. By default it deforms the map.
    This is useful for patterns that work on best a -1 to 1 coordinate system, and your map is smaller (or bigger).
    */
    PixelMapPtr normalize(bool keepAspectRatio = false) const
    {
        float min_x = FLT_MAX;
        float min_y = FLT_MAX;
        float max_x = -FLT_MAX;
        float max_y = -FLT_MAX;

        for (auto pos : *this)
        {
            if (pos.x < min_x)
                min_x = pos.x;
            if (pos.y < min_y)
                min_y = pos.y;
            if (pos.x > max_x)
                max_x = pos.x;
            if (pos.y > max_y)
                max_y = pos.y;
        }

        float scale_x;
        float scale_y;
        float x;
        float y;

        if (keepAspectRatio)
        {
            float scale = 2. / std::max(max_x - min_x, max_y - min_y);
            scale_x = scale;
            scale_y = scale;

            x = -1 - min_x * scale;
            y = -1 - min_y * scale;
        }
        else
        {
            scale_x = 2. / (max_x - min_x);
            scale_y = 2. / (max_y - min_y);

            x = -1 - min_x * scale_x;
            y = -1 - min_y * scale_y;
        }

        PixelMap result = *this;
        for (auto &pos : result)
        {
            pos.x = pos.x * scale_x + x;
            pos.y = pos.y * scale_y + y;
        }
        return std::make_shared<PixelMap>(result);
    }

    PixelMapPtr rotate(float angle, AngularUnits units = AngularUnits::DEG) const
    {
        angle = toRadians(angle, units);
        PixelMap result = *this;
        for (auto &pos : result)
        {
            float x = pos.x * cos(angle) - pos.y * sin(angle);
            float y = pos.x * sin(angle) + pos.y * cos(angle);
            pos.x = x;
            pos.y = y;
        }
        return std::make_shared<PixelMap>(result);
    }

    PixelMapPtr applyIndexMap(IndexMap *indexMap) const
    {
        PixelMap result;
        result.reserve(this->size());
        for (int i=0; i<this->size(); i++)
            result.push_back((*this)[indexMap->map(i)]);
        return std::make_shared<PixelMap>(result);
    }
};
