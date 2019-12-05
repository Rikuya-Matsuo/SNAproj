#include "Vector.h"

const Vector2D Vector2D::zero = Vector2D(0.0f, 0.0f);

Vector2D::Vector2D(float in_x, float in_y):
	x(in_x),
	y(in_y)
{
}

const Vector3D Vector3D::zero = Vector3D(0.0f, 0.0f, 0.0f);

Vector2D::operator Vector3D() const
{
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
	Vector2D ret(this->x, this->y);
	return ret;
}
