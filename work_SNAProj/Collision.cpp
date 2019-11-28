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
	// �ő�̊p�̍��W�̍X�V
	mMax.x = Common::Larger(mMax.x, point.x);
	mMax.y = Common::Larger(mMax.y, point.y);
	mMax.z = Common::Larger(mMax.z, point.z);

	// �ŏ�
	mMin.x = Common::Smaller(mMin.x, point.x);
	mMin.y = Common::Smaller(mMin.y, point.y);
	mMin.z = Common::Smaller(mMin.z, point.z);
}

bool AABB::IsPointInside(const Vector3D & point) const
{
	// �_���O���ɂ������
	bool outside =
		point.x < mMin.x ||
		point.y < mMin.y ||
		point.z < mMin.z ||
		point.x > mMax.x ||
		point.y > mMax.y ||
		point.z > mMax.z;

	// �O���ɂȂ��Ȃ�A�����ɂ���ɈႢ�Ȃ�
	return !outside;
}

float AABB::MinimumDistanceSq(const Vector3D & point) const
{
	// ���ꂼ��̎��ł̍����Ƃ�
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
