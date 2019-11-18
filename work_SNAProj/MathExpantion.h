#pragma once
#include <cmath>
#include "SDL/SDL_stdinc.h"

namespace MathExpantion
{
	float Cot(float angle)
	{
		return 1.0f / tanf(angle);
	}

	float Lerp(float a, float b, float rate)
	{
		return a + ((b - a) * rate);
	}

	float ToRadians(float degrees)
	{
		return degrees * (float)M_PI / 180.0f;
	}
}
