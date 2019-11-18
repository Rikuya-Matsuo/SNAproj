#include "Vector.h"

const Vector2D Vector2D::zero = Vector2D(0.0f, 0.0f);

Vector2D::Vector2D(float in_x, float in_y):
	x(in_x),
	y(in_y)
{
}

const Vector3D Vector3D::zero = Vector3D(0.0f, 0.0f, 0.0f);

Vector3D::Vector3D(float in_x, float in_y, float in_z):
	x(in_x),
	y(in_y),
	z(in_z)
{
}
