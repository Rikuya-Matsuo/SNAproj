#pragma once
#include "Effect.h"

class Texture;
class AnimationChips;

class AnimationEffect : public Effect
{
public:
	AnimationEffect(int priority, const std::string & animPath, int allNum, int xNum, int yNum, int chipW, int chipH, float secondPerFrame);
	~AnimationEffect();

	void UpdateActor0() override;
	void UpdateActor1() override;

	void OnBecomeActive() override;

	AnimationChips * GetAnimChips() const { return mAnimChip; }

	bool GetEndlessLoopFlag() const { return mEndlessLoopFlag; }

	void SetEndlessLoopFlag(bool value) { mEndlessLoopFlag = value; }

private:
	AnimationChips * mAnimChip;

	bool mEndlessLoopFlag;
};
