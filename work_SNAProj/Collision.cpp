#include "Collision.h"
#include "Common.h"

Sphere::Sphere(const Vector3D & center, float radius) :
	mCenter(center),
	mRadius(radius)
{
}

bool Sphere::IsPointInside(const Vector3D & point) const
{
	float distSq = (mCenter - point).LengthSq();
	return distSq <= mRadius * mRadius;
}

AABB::AABB()
{
	mMin = mMax = Vector3D::zero;
}

AABB::AABB(const Vector3D & min, const Vector3D & max):
	mMin(min),
	mMax(max)
{
}

void AABB::RenewalMinMax(const Vector3D & point)
{
	// 最大の角の座標の更新
	mMax.x = Common::Larger(mMax.x, point.x);
	mMax.y = Common::Larger(mMax.y, point.y);
	mMax.z = Common::Larger(mMax.z, point.z);

	// 最小
	mMin.x = Common::Smaller(mMin.x, point.x);
	mMin.y = Common::Smaller(mMin.y, point.y);
	mMin.z = Common::Smaller(mMin.z, point.z);
}

bool AABB::IsPointInside(const Vector3D & point) const
{
	// 点が外側にある条件
	bool outside =
		point.x < mMin.x ||
		point.y < mMin.y ||
		point.z < mMin.z ||
		point.x > mMax.x ||
		point.y > mMax.y ||
		point.z > mMax.z;

	// 外側にないなら、内側にあるに違いない
	return !outside;
}

float AABB::MinimumDistanceSq(const Vector3D & point) const
{
	// それぞれの軸での差をとる
	Vector3D v;
	v.x = Common::Larger(mMin.x - point.x, 0.0f);
	v.x = Common::Larger(v.x, point.x - mMax.x);

	v.y = Common::Larger(mMin.y - point.y, 0.0f);
	v.y = Common::Larger(v.y, point.y - mMax.y);

	v.z = Common::Larger(mMin.z - point.z, 0.0f);
	v.z = Common::Larger(v.z, point.z - mMax.z);

	return v.LengthSq();
}

bool Collision::CheckHit(const AABB & box1, const AABB & box2)
{
	bool notHit =
		box1.mMax.x < box2.mMin.x ||
		box1.mMax.y < box2.mMin.y ||
		box1.mMax.z < box2.mMin.z ||
		box2.mMax.x < box1.mMin.x ||
		box2.mMax.y < box1.mMin.y ||
		box2.mMax.z < box1.mMin.z;

	// 一つもtrueがなければ、二つは衝突している
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
