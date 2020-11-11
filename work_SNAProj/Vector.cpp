#include "Vector.h"
#include "Quaternion.h"

const Vector2D Vector2D::zero = Vector2D(0.0f, 0.0f);

Vector2D::Vector2D(float in_x, float in_y):
	x(in_x),
	y(in_y)
{
}

const Vector3D Vector3D::zero = Vector3D(0.0f, 0.0f, 0.0f);

Vector2D::operator Vector3D() const
{
	// x, yはそのまま代入。zは0.0fとする。
	Vector3D ret(this->x, this->y, 0.0f);
	return ret;
}

Vector3D::Vector3D(float in_x, float in_y, float in_z):
	x(in_x),
	y(in_y),
	z(in_z)
{
}

Vector3D::operator Vector2D() const
{
	// x, yのみ抽出
	Vector2D ret(this->x, this->y);
	return ret;
}

Vector3D Vector3D::Transform(const Vector3D & v, const Quaternion & q)
{
	Vector3D qv(q.x, q.y, q.z);
	Vector3D retVal = v;
	retVal += 2.0f * Vector3D::Cross(qv, Vector3D::Cross(qv, v) + q.w * v);
	return retVal;
}
