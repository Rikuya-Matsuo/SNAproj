#pragma once
#include "ComponentBase.h"
#include "BitFlagFunc.h"

// é¿ëïíÜÅEÅEÅE
class JumpComponent : public ComponentBase
{
public:
	JumpComponent(Actor * owner, float power = 100.0f);
	~JumpComponent();

	void Update() override;

	void SetJumpFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mJumpFlags, mTimingFlagMask); }

	void SetPower(float power) { mPower = power; }

private:
	typedef Uint8 FlagType;
	static const FlagType mTimingFlagMask;


	FlagType mJumpFlags;

	float mPower;
};

