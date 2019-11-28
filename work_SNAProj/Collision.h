#pragma once
#include "Vector.h"
#include "Common.h"

struct Sphere
{
	Sphere(const Vector3D& center, float radius = 1.0f);

	bool IsPointInside(const Vector3D& point) const;

	Vector3D mCenter;

	float mRadius;
};

struct AABB
{
	AABB();
	AABB(const Vector3D & min, const Vector3D & max);

	Vector3D mMin;
	Vector3D mMax;

	void InitMinMax(const Vector3D & point) { mMin = mMax = point; }

	// 角の座標を更新
	// 入力された角の座標と今の最大最小の角の座標を比較して、値を更新する
	void RenewalMinMax(const Vector3D & point);

	bool IsPointInside(const Vector3D& point) const;

	float MinimumDistanceSq(const Vector3D& point) const;
};

namespace Collision
{
	static bool CheckHit(const AABB & box1, const AABB & box2);

	static bool CheckHit(const Sphere & sphere, const AABB & box);

	static bool CheckHit(const Sphere & s1, const Sphere & s2);
}
