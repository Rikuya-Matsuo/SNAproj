#include "Camera.h"
#include "Actor.h"
#include "System.h"
#include "Input.h"

bool Camera::mWatchTargetFlag = true;

bool Camera::mChaseTargetFlag = true;

Camera::Camera(Actor * target):
	mTargetActor(target),
	mDistanceVector(Vector3D::zero)
{
	// 位置の初期化
	mPosition = Vector3D(0, 100, 100);

	// カメラに写す対象の位置を取得
	Vector3D targetVec = Vector3D::zero;
	if (target != nullptr)
	{
		targetVec = target->GetPosition();

		// ついでに距離ベクトルも初期化
		mDistanceVector = mPosition - targetVec;
	}
	mViewMatrix = Matrix4::CreateLookAt(mPosition, targetVec, Vector3D(0, 0, 1));

	// このカメラをアクティブに
	SetActive();
}

Camera::~Camera()
{
	// カメラの破棄をシステムに通告
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

	// アクターを目で追う
	if (mWatchTargetFlag && mTargetActor != nullptr)
	{
		mViewTarget = mTargetActor->GetPosition();
	}

	// アクターを追いかける
	if (mChaseTargetFlag && mTargetActor != nullptr)
	{
		mPosition = mTargetActor->GetPosition() + mDistanceVector;
	}

	// カメラ視線ベクトル、カメラ行列作成
	mViewVector = mViewTarget - mPosition;
	mViewMatrix = Matrix4::CreateLookAt(mPosition, mViewTarget, Vector3D(0.0f, 0.0f, 1.0f));
}

void Camera::Init(const Vector3D & cameraPos, const Vector3D & targetPos, const Vector3D & upVec)
{
	// 各種変数初期化
	mPosition = cameraPos;
	mViewTarget = targetPos;
	mViewVector = targetPos - cameraPos;

	mViewMatrix = Matrix4::CreateLookAt(cameraPos,
		targetPos,
		upVec);
}

void Camera::SetActive()
{
	// このカメラをアクティブカメラとしてセット
	System::GetInstance().SetActiveCamera(this);
}
