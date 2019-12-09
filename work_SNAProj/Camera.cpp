#include "Camera.h"
#include "Actor.h"
#include "System.h"

Camera::Camera(Actor * target):
	mTargetActor(target)
{
	mPosition = Vector3D(0, 100, 100);

	Vector3D targetVec = Vector3D::zero;
	if (target != nullptr)
	{
		targetVec = target->GetPosition();
	}
	mViewMatrix = Matrix4::CreateLookAt(mPosition, targetVec, Vector3D(0, 0, 1));

	SetActive();
}

Camera::~Camera()
{
	SDL_Log("A camera object is deleted.\n");
	System::GetInstance().ReportCameraDelete(this);
}

void Camera::Update()
{
	// カメラ視線ベクトル、カメラ行列作成
	mViewVector = mViewTarget - mPosition;
	mViewMatrix = Matrix4::CreateLookAt(mPosition, mViewTarget, Vector3D(0.0f, 0.0f, 1.0f));
}

void Camera::Init(const Vector3D & cameraPos, const Vector3D & targetPos, const Vector3D & upVec)
{
	mPosition = cameraPos;
	mViewTarget = targetPos;
	mViewVector = targetPos - cameraPos;

	mViewMatrix = Matrix4::CreateLookAt(cameraPos,
		targetPos,
		upVec);
}

void Camera::SetActive()
{
	System::GetInstance().SetActiveCamera(this);
}
