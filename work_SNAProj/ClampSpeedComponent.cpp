#include "ClampSpeedComponent.h"
#include "System.h"
#include "Common.h"

#include <cmath>

const Uint8 ClampSpeedComponent::mClampXFlagMask = 1 << 0;
const Uint8 ClampSpeedComponent::mClampYFlagMask = 1 << 1;
const Uint8 ClampSpeedComponent::mClampZFlagMask = 1 << 2;

ClampSpeedComponent::ClampSpeedComponent(Actor * owner, const Vector3D & limit):
	ComponentBase(owner, 400),
	mLimit(limit),
	mOwnerMoveVector(const_cast<Vector3D&>(mOwner->GetMoveVector())),
	mClampDirectionFlag(mClampXFlagMask | mClampYFlagMask | mClampZFlagMask)
{
}

ClampSpeedComponent::~ClampSpeedComponent()
{
}

void ClampSpeedComponent::SetClampDirectionFlags(bool x, bool y, bool z)
{
	// クランプする軸の設定
	Uint8 xMask = x ? mClampXFlagMask : 0;
	Uint8 yMask = y ? mClampYFlagMask : 0;
	Uint8 zMask = z ? mClampZFlagMask : 0;

	mClampDirectionFlag = xMask | yMask | zMask;
}

void ClampSpeedComponent::Update()
{
	const float deltaTime = System::GetInstance().GetDeltaTime();
	auto clamp = [deltaTime](float & speed, float limit)
	{
		// 制限値が万が一負の値になっていた時のため、絶対値を取る &
		// デルタタイムを考慮した値に制限値を調整する
		float absLim = fabsf(limit) * deltaTime;

		speed = Common::Clamp(speed, -absLim, absLim);
	};

	// 各軸の速度をクランプ
	if (mClampDirectionFlag & mClampXFlagMask)
	{
		clamp(mOwnerMoveVector.x, mLimit.x);
	}

	if (mClampDirectionFlag & mClampYFlagMask)
	{
		clamp(mOwnerMoveVector.y, mLimit.y);
	}

	if (mClampDirectionFlag & mClampZFlagMask)
	{
		clamp(mOwnerMoveVector.z, mLimit.z);
	}
}
