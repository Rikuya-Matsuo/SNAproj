#include "JumpComponent.h"
#include "Actor.h"
#include "System.h"

const float JumpComponent::mPushUpTimeCount = 0.5f;

const JumpComponent::FlagType JumpComponent::mTimingFlagMask = 1 << 0;

JumpComponent::JumpComponent(Actor * owner, float power):
	ComponentBase(owner, 100),
	mJumpFlags(0),
	mPower(power),
	mTimer(0.0f)
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

		moveVec.z += mPower * System::GetInstance().GetDeltaTime();

		mJumpFlags &= ~mTimingFlagMask;

		/*mTimer += System::GetInstance().GetDeltaTime();
		if (mTimer > mPushUpTimeCount)
		{

			mTimer = 0.0f;
		}*/

		mOwner->SetMoveVector(moveVec);
	}
}
