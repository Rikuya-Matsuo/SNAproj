#pragma once
#include "SDL/SDL.h"

struct GamePadAxis
{
	GamePadAxis();

	Sint16 x;
	Sint16 y;

	float GetFloatX() const;
	float GetFloatY() const;
};
