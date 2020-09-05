#pragma once
#include "ComponentBase.h"

class AutoMoveComponent : public ComponentBase
{
public:
	AutoMoveComponent(Actor * owner, const Vector3D & velocity = Vector3D::zero);
	~AutoMoveComponent();

	void Update() override;

	void SetVelocity(const Vector3D & vec);
	const Vector3D & GetVelocity() const { return mVelocity; }

	void SetReverseFlag(bool x, bool y, bool z);

	void ReverseVelocity();

	bool IsNowReverse() const { return mFlags_AMC & mReverseFlagMask; }

private:
	static const Uint8 mReverseFlagMask;
	static const Uint8 mReverseXFlagMask;
	static const Uint8 mReverseYFlagMask;
	static const Uint8 mReverseZFlagMask;


	Uint8 mFlags_AMC;

	Vector3D mVelocity;
};

