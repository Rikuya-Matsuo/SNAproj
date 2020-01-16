#pragma once
#include "Actor.h"

class Effect : public Actor
{
public:
	Effect(const std::string & texPath);
	~Effect();

	void SetAppearSecond(float second) { mAppearSecond = second; }

protected:
	static const float mDepth;

	// èoåªéûä‘
	float mAppearSecond;

	float mTimer;

	void UpdateActor0() override;

	void OnBecomeActive() override;
};

