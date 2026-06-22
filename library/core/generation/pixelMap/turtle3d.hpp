#pragma once

#include <array>
#include <cmath>

#include "pixelMap.hpp"

class Turtle3d : public PixelMap3d
{
public:
    Turtle3d()
        : pos_x(0.0f)
        , pos_y(0.0f)
        , pos_z(0.0f)
        , rotation{{1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f}}
    {
    }

    void yaw(float angle)
    {
        const float alpha = angle * 2.0f * M_PI / 360.0f;
        const Matrix3 yawRotation{{
            std::cos(alpha), -std::sin(alpha), 0.0f,
            std::sin(alpha),  std::cos(alpha), 0.0f,
            0.0f,             0.0f,            1.0f}};

        rotation = multiply(yawRotation, rotation);
    }

    void pitch(float angle)
    {
        const float beta = angle * 2.0f * M_PI / 360.0f;
        const Matrix3 pitchRotation{{
            std::cos(beta),  0.0f, std::sin(beta),
            0.0f,            1.0f, 0.0f,
            -std::sin(beta), 0.0f, std::cos(beta)}};

        rotation = multiply(pitchRotation, rotation);
    }

    void roll(float angle)
    {
        const float gamma = angle * 2.0f * M_PI / 360.0f;
        const Matrix3 rollRotation{{
            1.0f, 0.0f,             0.0f,
            0.0f, std::cos(gamma), -std::sin(gamma),
            0.0f, std::sin(gamma),  std::cos(gamma)}};

        rotation = multiply(rollRotation, rotation);
    }

    void move(float distance, bool addPositionToTrail = true)
    {
        pos_x += distance * rotation[0];
        pos_y += distance * rotation[1];
        pos_z += distance * rotation[2];

        if (addPositionToTrail)
        {
            this->addPositionToTrail();
        }
    }

    void addPositionToTrail()
    {
        push_back({.x = pos_x, .y = pos_y, .z = pos_z});
    }

    void clearTrail()
    {
        clear();
    }

    void setPosition(float x, float y, float z)
    {
        pos_x = x;
        pos_y = y;
        pos_z = z;
    }

    void setRotation(float yawAngle, float pitchAngle, float rollAngle)
    {
        rotation = Matrix3{{1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f}};
        yaw(yawAngle);
        pitch(pitchAngle);
        roll(rollAngle);
    }

    float pos_x;
    float pos_y;
    float pos_z;

    using Matrix3 = std::array<float, 9>;

private:
    static Matrix3 multiply(const Matrix3 &left, const Matrix3 &right)
    {
        Matrix3 result{};

        for (int row = 0; row < 3; ++row)
        {
            for (int column = 0; column < 3; ++column)
            {
                result[row * 3 + column] =
                    left[row * 3 + 0] * right[0 * 3 + column] +
                    left[row * 3 + 1] * right[1 * 3 + column] +
                    left[row * 3 + 2] * right[2 * 3 + column];
            }
        }

        return result;
    }

    Matrix3 rotation;
};