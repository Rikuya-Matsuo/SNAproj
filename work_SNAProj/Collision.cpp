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
