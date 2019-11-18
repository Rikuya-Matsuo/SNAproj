#pragma once
#include "SDL/SDL.h"
#include "Vector.h"
#include "MathExpantion.h"
#include <cstring>
#include <cmath>

static float m4Ident[4][4] =
{
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f }
};

// 4x4 Matrix
class Matrix4
{
public:
	float mat[4][4];

	Matrix4()
	{
		*this = Matrix4::Identity;
	}

	explicit Matrix4(float inMat[4][4])
	{
		memcpy(mat, inMat, 16 * sizeof(float));
	}

	// Cast to a const float pointer
	const float* GetAsFloatPtr() const
	{
		return reinterpret_cast<const float*>(&mat[0][0]);
	}

	// Matrix multiplication (a * b)
	friend Matrix4 operator*(const Matrix4& a, const Matrix4& b)
	{
		Matrix4 retVal;
		// row 0
		retVal.mat[0][0] =
			a.mat[0][0] * b.mat[0][0] +
			a.mat[0][1] * b.mat[1][0] +
			a.mat[0][2] * b.mat[2][0] +
			a.mat[0][3] * b.mat[3][0];

		retVal.mat[0][1] =
			a.mat[0][0] * b.mat[0][1] +
			a.mat[0][1] * b.mat[1][1] +
			a.mat[0][2] * b.mat[2][1] +
			a.mat[0][3] * b.mat[3][1];

		retVal.mat[0][2] =
			a.mat[0][0] * b.mat[0][2] +
			a.mat[0][1] * b.mat[1][2] +
			a.mat[0][2] * b.mat[2][2] +
			a.mat[0][3] * b.mat[3][2];

		retVal.mat[0][3] =
			a.mat[0][0] * b.mat[0][3] +
			a.mat[0][1] * b.mat[1][3] +
			a.mat[0][2] * b.mat[2][3] +
			a.mat[0][3] * b.mat[3][3];

		// row 1
		retVal.mat[1][0] =
			a.mat[1][0] * b.mat[0][0] +
			a.mat[1][1] * b.mat[1][0] +
			a.mat[1][2] * b.mat[2][0] +
			a.mat[1][3] * b.mat[3][0];

		retVal.mat[1][1] =
			a.mat[1][0] * b.mat[0][1] +
			a.mat[1][1] * b.mat[1][1] +
			a.mat[1][2] * b.mat[2][1] +
			a.mat[1][3] * b.mat[3][1];

		retVal.mat[1][2] =
			a.mat[1][0] * b.mat[0][2] +
			a.mat[1][1] * b.mat[1][2] +
			a.mat[1][2] * b.mat[2][2] +
			a.mat[1][3] * b.mat[3][2];

		retVal.mat[1][3] =
			a.mat[1][0] * b.mat[0][3] +
			a.mat[1][1] * b.mat[1][3] +
			a.mat[1][2] * b.mat[2][3] +
			a.mat[1][3] * b.mat[3][3];

		// row 2
		retVal.mat[2][0] =
			a.mat[2][0] * b.mat[0][0] +
			a.mat[2][1] * b.mat[1][0] +
			a.mat[2][2] * b.mat[2][0] +
			a.mat[2][3] * b.mat[3][0];

		retVal.mat[2][1] =
			a.mat[2][0] * b.mat[0][1] +
			a.mat[2][1] * b.mat[1][1] +
			a.mat[2][2] * b.mat[2][1] +
			a.mat[2][3] * b.mat[3][1];

		retVal.mat[2][2] =
			a.mat[2][0] * b.mat[0][2] +
			a.mat[2][1] * b.mat[1][2] +
			a.mat[2][2] * b.mat[2][2] +
			a.mat[2][3] * b.mat[3][2];

		retVal.mat[2][3] =
			a.mat[2][0] * b.mat[0][3] +
			a.mat[2][1] * b.mat[1][3] +
			a.mat[2][2] * b.mat[2][3] +
			a.mat[2][3] * b.mat[3][3];

		// row 3
		retVal.mat[3][0] =
			a.mat[3][0] * b.mat[0][0] +
			a.mat[3][1] * b.mat[1][0] +
			a.mat[3][2] * b.mat[2][0] +
			a.mat[3][3] * b.mat[3][0];

		retVal.mat[3][1] =
			a.mat[3][0] * b.mat[0][1] +
			a.mat[3][1] * b.mat[1][1] +
			a.mat[3][2] * b.mat[2][1] +
			a.mat[3][3] * b.mat[3][1];

		retVal.mat[3][2] =
			a.mat[3][0] * b.mat[0][2] +
			a.mat[3][1] * b.mat[1][2] +
			a.mat[3][2] * b.mat[2][2] +
			a.mat[3][3] * b.mat[3][2];

		retVal.mat[3][3] =
			a.mat[3][0] * b.mat[0][3] +
			a.mat[3][1] * b.mat[1][3] +
			a.mat[3][2] * b.mat[2][3] +
			a.mat[3][3] * b.mat[3][3];

		return retVal;
	}

	Matrix4& operator*=(const Matrix4& right)
	{
		*this = *this * right;
		return *this;
	}

	// Invert the matrix - super slow
	void Invert();

	// Get the translation component of the matrix
	Vector3D GetTranslation() const
	{
		return Vector3D(mat[3][0], mat[3][1], mat[3][2]);
	}

	// Get the X axis of the matrix (forward)
	Vector3D GetXAxis() const
	{
		return Vector3D::Normalize(Vector3D(mat[0][0], mat[0][1], mat[0][2]));
	}

	// Get the Y axis of the matrix (left)
	Vector3D GetYAxis() const
	{
		return Vector3D::Normalize(Vector3D(mat[1][0], mat[1][1], mat[1][2]));
	}

	// Get the Z axis of the matrix (up)
	Vector3D GetZAxis() const
	{
		return Vector3D::Normalize(Vector3D(mat[2][0], mat[2][1], mat[2][2]));
	}

	// Extract the scale component from the matrix
	Vector3D GetScale() const
	{
		Vector3D retVal;
		retVal.x = Vector3D(mat[0][0], mat[0][1], mat[0][2]).Length();
		retVal.y = Vector3D(mat[1][0], mat[1][1], mat[1][2]).Length();
		retVal.z = Vector3D(mat[2][0], mat[2][1], mat[2][2]).Length();
		return retVal;
	}

	// Create a scale matrix with x, y, and z scales
	static Matrix4 CreateScale(float xScale, float yScale, float zScale)
	{
		float temp[4][4] =
		{
			{ xScale, 0.0f, 0.0f, 0.0f },
			{ 0.0f, yScale, 0.0f, 0.0f },
			{ 0.0f, 0.0f, zScale, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		};
		return Matrix4(temp);
	}

	static Matrix4 CreateScale(const Vector3D& scaleVector)
	{
		return CreateScale(scaleVector.x, scaleVector.y, scaleVector.z);
	}

	// Create a scale matrix with a uniform factor
	static Matrix4 CreateScale(float scale)
	{
		return CreateScale(scale, scale, scale);
	}

	// Rotation about x-axis
	static Matrix4 CreateRotationX(float theta)
	{
		float temp[4][4] =
		{
			{ 1.0f, 0.0f, 0.0f , 0.0f },
			{ 0.0f, cosf(theta), sinf(theta), 0.0f },
			{ 0.0f, -sinf(theta), cosf(theta), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4(temp);
	}

	// Rotation about y-axis
	static Matrix4 CreateRotationY(float theta)
	{
		float temp[4][4] =
		{
			{ cosf(theta), 0.0f, -sinf(theta), 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ sinf(theta), 0.0f, cosf(theta), 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4(temp);
	}

	// Rotation about z-axis
	static Matrix4 CreateRotationZ(float theta)
	{
		float temp[4][4] =
		{
			{ cosf(theta), sinf(theta), 0.0f, 0.0f },
			{ -sinf(theta), cosf(theta), 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		return Matrix4(temp);
	}

	// Create a rotation matrix from a quaternion
	static Matrix4 CreateFromQuaternion(const class Quaternion& q);

	static Matrix4 CreateTranslation(const Vector3D& trans)
	{
		float temp[4][4] =
		{
			{ 1.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ trans.x, trans.y, trans.z, 1.0f }
		};
		return Matrix4(temp);
	}

	static Matrix4 CreateLookAt(const Vector3D& eye, const Vector3D& target, const Vector3D& up)
	{
		Vector3D zaxis = Vector3D::Normalize(target - eye);
		Vector3D xaxis = Vector3D::Normalize(Vector3D::Cross(up, zaxis));
		Vector3D yaxis = Vector3D::Normalize(Vector3D::Cross(zaxis, xaxis));
		Vector3D trans;
		trans.x = -Vector3D::Dot(xaxis, eye);
		trans.y = -Vector3D::Dot(yaxis, eye);
		trans.z = -Vector3D::Dot(zaxis, eye);

		float temp[4][4] =
		{
			{ xaxis.x, yaxis.x, zaxis.x, 0.0f },
			{ xaxis.y, yaxis.y, zaxis.y, 0.0f },
			{ xaxis.z, yaxis.z, zaxis.z, 0.0f },
			{ trans.x, trans.y, trans.z, 1.0f }
		};
		return Matrix4(temp);
	}

	static Matrix4 CreateOrtho(float width, float height, float near, float far)
	{
		float temp[4][4] =
		{
			{ 2.0f / width, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / height, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f / (far - near), 0.0f },
			{ 0.0f, 0.0f, near / (near - far), 1.0f }
		};
		return Matrix4(temp);
	}

	static Matrix4 CreatePerspectiveFOV(float fovY, float width, float height, float near, float far)
	{
		float yScale = MathExpantion::Cot(fovY / 2.0f);
		float xScale = yScale * height / width;
		float temp[4][4] =
		{
			{ xScale, 0.0f, 0.0f, 0.0f },
			{ 0.0f, yScale, 0.0f, 0.0f },
			{ 0.0f, 0.0f, far / (far - near), 1.0f },
			{ 0.0f, 0.0f, -near * far / (far - near), 0.0f }
		};
		return Matrix4(temp);
	}

	// Create "Simple" View-Projection Matrix from Chapter 6
	static Matrix4 CreateSimpleViewProj(float width, float height)
	{
		float temp[4][4] =
		{
			{ 2.0f / width, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 2.0f / height, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f }
		};
		return Matrix4(temp);
	}

	static const Matrix4 Identity;
};
