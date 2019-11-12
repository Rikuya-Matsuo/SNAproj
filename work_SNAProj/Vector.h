#pragma once

class Vector2D
{
public:
	Vector2D(float in_x, float in_y);

	float x;
	float y;

	static const Vector2D zero;
};
