#pragma once
#include "Vector.h"
#include "Matrix.h"

class Actor;

class Camera
{
public:
	Camera(Actor * target = nullptr);
	virtual ~Camera();

	void Update();
	void Init(const Vector3D & cameraPos, const Vector3D & targetPos, const Vector3D & upVec);

	void SetTargetActor(const Actor * target) { mTargetActor = const_cast<Actor *>(target); }
	void SetActive();

	const Matrix4 & GetViewMatrix() const { return mViewMatrix; }
	const Vector3D & GetViewVector() const { return mViewVector; }

private:
	Actor * mTargetActor;
	
	Matrix4 mViewMatrix;

	Vector3D mPosition;

	Vector3D mViewTarget;

	Vector3D mViewVector;
};
