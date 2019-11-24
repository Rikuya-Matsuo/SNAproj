#include "Collision.h"

AABB::AABB()
{
}

AABB::AABB(const Vector3D & min, const Vector3D & max):
	mMin(min),
	mMax(max)
{
}

void AABB::RenewalMinMax(const Vector3D & point)
{
	// ç≈ëÂÇÃäpÇÃç¿ïWÇÃçXêV
	mMax.x = Common::Larger(mMax.x, point.x);
	mMax.y = Common::Larger(mMax.y, point.y);
	mMax.z = Common::Larger(mMax.z, point.z);

	// ç≈è¨
	mMin.x = Common::Smaller(mMin.x, point.x);
	mMin.y = Common::Smaller(mMin.y, point.y);
	mMin.z = Common::Smaller(mMin.z, point.z);
}
