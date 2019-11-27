#pragma once
#include "Vector.h"
#include "Common.h"

struct Sphere
{
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

};
