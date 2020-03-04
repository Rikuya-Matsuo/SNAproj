#include "JumpComponent.h"
#include "Actor.h"
#include "System.h"
#include "PhysicManager.h"

#ifdef DEBUG_SNA
#include "Input.h"
bool debugFlag = false;
#endif // DEBUG_SNA


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
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_LCTRL))
	{
		debugFlag = !debugFlag;
	}

	if (mJumpFlags & mTimingFlagMask)
	{
		if (debugFlag)
		{
			SDL_Delay(0);
		}

		Vector3D moveVec = mOwner->GetMoveVector();

		//float rate = System::GetInstance().GetDeltaTime() / System::GetInstance().GetAverageDeltaTime();
		moveVec.z += mPower;

		mJumpFlags &= ~mTimingFlagMask;

		/*
		mTimer += System::GetInstance().GetDeltaTime();
		if (mTimer > mPushUpTimeCount)
		{

			mTimer = 0.0f;
		}
		*/

		mOwner->SetMoveVector(moveVec);
	}
}

void JumpComponent::SetJumpHeight(float height, float timeToGetHighest)
{
	// 時間が0だった場合エラーが起きるので、その対策も兼ねて時間を変更する
	// 1フレームの秒数最大値とする
	if (!timeToGetHighest)
	{
		timeToGetHighest = System::mMaxDeltaTime;
	}

	// 鉛直投げ上げの公式
	// （変位）　=　（初速度 * 時間） - (1/2)（重力加速度 * 時間 * 時間）
	// より、初速度（ジャンプパワーと呼称している）が求められる
	mPower = (1.0f / 2.0f) * PhysicManager::mGravityAcceleration * timeToGetHighest + (height / timeToGetHighest);
}
