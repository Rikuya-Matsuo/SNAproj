#include "AutoMoveComponent.h"
#include "System.h"

const Uint8 AutoMoveComponent::mReverseFlagMask = 1 << 0;
const Uint8 AutoMoveComponent::mReverseXFlagMask = 1 << 1;
const Uint8 AutoMoveComponent::mReverseYFlagMask = 1 << 2;
const Uint8 AutoMoveComponent::mReverseZFlagMask = 1 << 3;

AutoMoveComponent::AutoMoveComponent(Actor * owner, const Vector3D & velocity):
	ComponentBase(owner),
	mFlags_AMC(mReverseXFlagMask | mReverseYFlagMask | mReverseZFlagMask),
	mVelocity(velocity)
{
}

AutoMoveComponent::~AutoMoveComponent()
{
}

void AutoMoveComponent::Update()
{
	Vector3D v = mOwner->GetMoveVector();
	v += mVelocity * System::GetInstance().GetDeltaTime();
	mOwner->SetMoveVector(v);
}

void AutoMoveComponent::SetVelocity(const Vector3D & vec)
{
	mVelocity = vec;
	mFlags_AMC &= ~mReverseFlagMask;
}

void AutoMoveComponent::SetReverseFlag(bool x, bool y, bool z)
{
	Uint8 xMask = x ? mReverseXFlagMask : 0;
	Uint8 yMask = y ? mReverseYFlagMask : 0;
	Uint8 zMask = z ? mReverseZFlagMask : 0;

	// 反転フラグを除くすべてを初期化
	mFlags_AMC &= mReverseFlagMask;

	mFlags_AMC |= (xMask | yMask | zMask);
}

void AutoMoveComponent::ReverseVelocity()
{
	if (mFlags_AMC & mReverseXFlagMask)
	{
		mVelocity.x *= -1;
	}

	if (mFlags_AMC & mReverseYFlagMask)
	{
		mVelocity.y *= -1;
	}

	if (mFlags_AMC & mReverseZFlagMask)
	{
		mVelocity.z *= -1;
	}

	mFlags_AMC ^= mReverseFlagMask;
}
