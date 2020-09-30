#pragma once
#include "ComponentBase.h"

// 1秒に進む速さを制限するコンポーネントクラス
class ClampSpeedComponent : ComponentBase
{
public:
	ClampSpeedComponent(Actor * owner, const Vector3D & limit = Vector3D(100, 100, 100));
	~ClampSpeedComponent();

	void SetClampDirectionFlags(bool x, bool y, bool z);

	void Update() override;

	const Vector3D & GetLimit() const { return mLimit; }
	void SetLimit(const Vector3D & limit) { mLimit = limit; }

private:
	static const Uint8 mClampXFlagMask;
	static const Uint8 mClampYFlagMask;
	static const Uint8 mClampZFlagMask;

	Uint8 mClampDirectionFlag;

	Vector3D mLimit;

	Vector3D & mOwnerMoveVector;
};
