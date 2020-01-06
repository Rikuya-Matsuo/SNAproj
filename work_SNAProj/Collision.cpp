#include "Collision.h"
#include "Common.h"
#include <array>

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

void AABB::Rotate(const Quaternion & q)
{
	// ボックスの8つの頂点の配列作る
	std::array<Vector3D, 8> points;
	// 最小値は常にコーナーである
	points[0] = mMin;
	// 2つの最小値と1個の最大値の並べ替え
	points[1] = Vector3D(mMax.x, mMin.y, mMin.z);
	points[2] = Vector3D(mMin.x, mMax.y, mMin.z);
	points[3] = Vector3D(mMin.x, mMin.y, mMax.z);
	// 2つの最大値と1個の最小値の並べ替え
	points[4] = Vector3D(mMin.x, mMax.y, mMax.z);
	points[5] = Vector3D(mMax.x, mMin.y, mMax.z);
	points[6] = Vector3D(mMax.x, mMax.y, mMin.z);
	// 最大値は常にコーナーである
	points[7] = Vector3D(mMax);

	// 最初の点を回転
	Vector3D p = Vector3D::Transform(points[0], q);
	// 最小値と最大値を求めるために最初の点を入れてリセット
	mMin = p;
	mMax = p;
	// 回転によって最大値と最小値を求めなおす。
	for (size_t i = 1; i < points.size(); i++)
	{
		p = Vector3D::Transform(points[i], q);
		RenewalMinMax(p);
	}
}
