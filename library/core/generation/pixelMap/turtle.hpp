#pragma once

#include <cmath>

#include "pixelMap.hpp"

class Turtle : public PixelMap
{
public:
	Turtle(float pos_x = 0.0f, float pos_y = 0.0f, float direction = 0.0f)
		: pos_x(pos_x)
		, pos_y(pos_y)
		, direction(direction)
	{
	}

	void turn(float angle)
	{
		direction += angle;

		if (direction < 0.0f)
		{
			direction += 360.0f;
		}

		if (direction >= 360.0f)
		{
			direction -= 360.0f;
		}
	}

	void move(float distance, bool addPositionToTrail = true)
	{
		pos_x += distance * std::cos(direction * 2.0f * M_PI / 360.0f);
		pos_y += distance * std::sin(direction * 2.0f * M_PI / 360.0f);

		if (addPositionToTrail)
		{
			this->addPositionToTrail();
		}
	}

	void addPositionToTrail()
	{
		push_back({.x = pos_x, .y = pos_y});
	}

	void clearTrail()
	{
		clear();
	}

	void setPosition(float newPosX, float newPosY)
	{
		pos_x = newPosX;
		pos_y = newPosY;
	}

	void setDirection(float newDirection)
	{
		direction = newDirection;
	}

	float pos_x;
	float pos_y;
	float direction;
};
