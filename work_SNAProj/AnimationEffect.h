#pragma once
#include "Effect.h"

class Texture;
class AnimationChips;

class AnimationEffect : public Effect
{
public:
	AnimationEffect(int priority, const std::string & animPath, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame);
	~AnimationEffect();

private:
	AnimationChips * mAnimChip;
};
