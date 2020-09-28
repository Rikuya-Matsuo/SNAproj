#pragma once
#include "SDL/SDL.h"
#include <vector>

class AnimationTextureOfSDL
{
public:
	AnimationTextureOfSDL();

private:
	std::vector<SDL_Texture *> mChipTextures;
};

