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

	// �p�̍��W���X�V
	// ���͂��ꂽ�p�̍��W�ƍ��̍ő�ŏ��̊p�̍��W���r���āA�l���X�V����
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


// Collision�֐��Q
bool Collision::CheckHit(const AABB & box1, const AABB & box2)
{
	bool notHit =
		box1.mMax.x < box2.mMin.x ||
		box1.mMax.y < box2.mMin.y ||
		box1.mMax.z < box2.mMin.z ||
		box2.mMax.x < box1.mMin.x ||
		box2.mMax.y < box1.mMin.y ||
		box2.mMax.z < box1.mMin.z;

	// ���true���Ȃ���΁A��͏Փ˂��Ă���
	return !notHit;
}

bool Collision::CheckHit(const Sphere & sphere, const AABB & box)
{
	float distanceSq = box.MinimumDistanceSq(sphere.mCenter);
	return distanceSq <= (sphere.mRadius * sphere.mRadius);
}

bool Collision::CheckHit(const Sphere & s1, const Sphere & s2)
{
	float radiusSum = s1.mRadius + s2.mRadius;
	float distanceSq = (s1.mCenter - s2.mCenter).LengthSq();
	return distanceSq <= (radiusSum * radiusSum);
}
