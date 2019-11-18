#pragma once
#include <cmath>

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
}
