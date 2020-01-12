#pragma once
#include "ComponentBase.h"

class JumpComponent : public ComponentBase
{
public:
	JumpComponent(Actor * owner, float power = 500.0f);
	~JumpComponent();

	void Update() override;

	void Jump() { mJumpFlags |= mTimingFlagMask; }

	// その高さのジャンプができるだけのPowerにセットする
	void SetJumpHeight(float height, float timeToGetHighest);

	void SetPower(float power) { mPower = power; }

private:
	typedef Uint8 FlagType;
	static const FlagType mTimingFlagMask;


	static const float mPushUpTimeCount;

	FlagType mJumpFlags;

	float mPower;

	float mTimer;
};

