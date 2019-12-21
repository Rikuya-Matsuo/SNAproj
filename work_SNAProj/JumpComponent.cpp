#include "JumpComponent.h"
#include "Actor.h"

const JumpComponent::FlagType JumpComponent::mTimingFlagMask = 1 << 0;

JumpComponent::JumpComponent(Actor * owner, float power):
	ComponentBase(owner, 100),
	mPower(power)
{

}

JumpComponent::~JumpComponent()
{
}

void JumpComponent::Update()
{
	if (mJumpFlags & mTimingFlagMask)
	{
		Vector3D moveVec = mOwner->GetMoveVector();

		moveVec += mPower;

		mJumpFlags &= ~mTimingFlagMask;
	}
}
