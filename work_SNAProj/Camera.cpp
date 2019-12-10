#include "Camera.h"
#include "Actor.h"
#include "System.h"
#include "Input.h"

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
#ifdef DEBUG_SNA
	Input& inp = Input::GetInstance();

	float speed = 50.0f * System::GetInstance().GetDeltaTime();
	// デバッグ用カメラ操作
	if (inp.GetKey(SDL_SCANCODE_LSHIFT))
	{
		if (inp.GetKey(SDL_SCANCODE_LEFT))
		{
			mPosition.x -= speed;
		}
		if (inp.GetKey(SDL_SCANCODE_RIGHT))
		{
			mPosition.x += speed;
		}

		bool rshift = false;
		if (inp.GetKey(SDL_SCANCODE_RSHIFT))
		{
			rshift = true;
		}

		if (inp.GetKey(SDL_SCANCODE_UP))
		{
			rshift ?
				mPosition.y -= speed :
				mPosition.z += speed;
		}
		if (inp.GetKey(SDL_SCANCODE_DOWN))
		{
			rshift ?
				mPosition.y += speed :
				mPosition.z -= speed;
		}
	}
#endif

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
