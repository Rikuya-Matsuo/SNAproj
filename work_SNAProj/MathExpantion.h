#pragma once
#include <cmath>
#include "SDL/SDL_stdinc.h"

namespace MathExpantion
{
	static float Cot(float angle)
	{
		return 1.0f / tanf(angle);
	}

	static float Lerp(float a, float b, float rate)
	{
		return a + ((b - a) * rate);
	}

	static float ToRadians(float degrees)
	{
		return degrees * (float)M_PI / 180.0f;
	}
}
