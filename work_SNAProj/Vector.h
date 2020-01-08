#pragma once
#include <cmath>

class Vector3D;

class Vector2D
{
public:
	Vector2D(float in_x = 0.0f, float in_y = 0.0f);

	float x;
	float y;

	static const Vector2D zero;

	operator Vector3D() const;
};

class Vector3D
{
public:
	Vector3D(float in_x = 0.0f, float in_y = 0.0f, float in_z = 0.0f);

	float x;
	float y;
	float z;

	static const Vector3D zero;

	// Vector addition (a + b)
	friend Vector3D operator+(const Vector3D& a, const Vector3D& b)
	{
		return Vector3D(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	// Vector subtraction (a - b)
	friend Vector3D operator-(const Vector3D& a, const Vector3D& b)
	{
		return Vector3D(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	// Component-wise multiplication
	friend Vector3D operator*(const Vector3D& left, const Vector3D& right)
	{
		return Vector3D(left.x * right.x, left.y * right.y, left.z * right.z);
	}

	// Scalar multiplication
	friend Vector3D operator*(const Vector3D& vec, float scalar)
	{
		return Vector3D(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	// Scalar multiplication
	friend Vector3D operator*(float scalar, const Vector3D& vec)
	{
		return Vector3D(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	friend bool operator==(const Vector3D& left, const Vector3D& right)
	{
		return
			left.x == right.x &&
			left.y == right.y &&
			left.z == right.z;
	}

	friend bool operator!=(const Vector3D& left, const Vector3D& right)
	{
		return !(left == right);
	}

	// Scalar *=
	Vector3D& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	// Vector +=
	Vector3D& operator+=(const Vector3D& right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		return *this;
	}

	// Vector -=
	Vector3D& operator-=(const Vector3D& right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
		return *this;
	}

	// Cast
	operator Vector2D() const;

	// Dot product between two vectors (a dot b)
	static float Dot(const Vector3D& a, const Vector3D& b)
	{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
	}

	// Cross product between two vectors (a cross b)
	static Vector3D Cross(const Vector3D& a, const Vector3D& b)
	{
		Vector3D temp;
		temp.x = a.y * b.z - a.z * b.y;
		temp.y = a.z * b.x - a.x * b.z;
		temp.z = a.x * b.y - a.y * b.x;
		return temp;
	}

	float LengthSq() const
	{
		return (x * x + y * y + z * z);
	}

	float Length() const
	{
		return sqrtf(LengthSq());
	}

	void Normalize()
	{
		float len = Length();
		x /= len;
		y /= len;
		z /= len;
	}

	static Vector3D Normalize(const Vector3D& vec)
	{
		Vector3D tmp = vec;
		tmp.Normalize();
		return tmp;
	}

	static Vector3D Lerp(const Vector3D& a, const Vector3D& b, float rate)
	{
		return Vector3D(a + ((b - a) *rate));
	}

	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&x);
	}

	// Transform a Vector3 by a quaternion
	static Vector3D Transform(const Vector3D& v, const class Quaternion& q);
};
