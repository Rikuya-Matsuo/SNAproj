#include "Player.h"
#include "Common.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"
#include "Mesh.h"
#include "System.h"
#include "Renderer.h"

Player::Player():
	Actor(),
	mLandingFlag(false),
	mDetectGroundFlag(false)
{
	// メッシュのロード
	MeshComponent * mc = new MeshComponent(this);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh");
	mMesh->LoadDivTexture("Assets/NinjaStay.png", System::GetInstance().GetRenderer(),
		10, 10, 1, 128, 128, 0.07f, 0);
	mc->SetMesh(mMesh);

	// コライダーの設定
	mBoxCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Player);
	mBoxCollider->SetObjectBox(mMesh->GetCollisionBox());

	AABB box = mMesh->GetCollisionBox();
	const Vector3D boxSize = box.mMax - box.mMin;
	box.mMax.z -= boxSize.z * 0.8f;
	box.mMin.z -= boxSize.z * 0.2f;
	BoxColliderComponent * groundChecker = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	groundChecker->SetObjectBox(box);

	mInputComponent = new InputMoveComponent(this);
	mInputComponent->SetVerticalAxis(mInputComponent->AxisEnum_z);
	
	const float speed = 500.0f;
	mInputComponent->SetHorizontalSpeed(speed);
	mInputComponent->SetVerticalSpeed(-speed);

	// 最大速度を調整
	mLimitSpeed = Vector3D(100.0f, 0.0f, 100.0f);

	// 落下スピード割合の調整
	mFallSpeedRate = 2.0f;

	// プレイヤーであることを示すフラグ
	mFlags |= mPlayerFlagMask;
}

Player::~Player()
{
	Common::DeleteContainerOfPointer(mComponentList);

	SDL_Log("Player is deleted\n");
}

void Player::UpdateActor0()
{
	if (!mDetectGroundFlag)
	{
		mLandingFlag = false;

		SetAffectGravityFlag(true);
	}

	mDetectGroundFlag = false;
}

void Player::UpdateActor1()
{
	// ジャンプ時に重力かける
	if (false && mInputComponent->GetUpKey())
	{
		mLandingFlag = false;

		SetAffectGravityFlag(true);
	}

	// ブレーキ
	if (!mInputComponent->GetHorizonInputFlag())
	{
		mMoveVector.x *= 0.05f;

		if (fabsf(mMoveVector.x) < 0.001f)
		{
			mMoveVector.x = 0.0f;
		}
	}

	/*if (!mInputComponent->GetVerticalInputFlag())
	{
		mMoveVector.z *= 0.05f;

		if (fabsf(mMoveVector.z) < 0.001f)
		{
			mMoveVector.z = 0.0f;
		}
	}*/

	// 奥行きの情報を常に0に
	mPosition.y = 0.0f;

	// 基底クラスのほうも呼ぶ
	Actor::UpdateActor1();
}

void Player::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	if (caller->GetColliderAttribute() == ColliderAttribute::ColAtt_Detector)
	{
		if (mPushedVector.z > 0)
		{
			mLandingFlag = true;

			mDetectGroundFlag = true;

			SetAffectGravityFlag(false);

			SDL_Log("Player : Detect Ground.\n");
		}
		return;
	}

	// すでにその方向への押し返しが働いている場合は、押し返しを無効化する
	// 着地中ならば常に押し返しを無効化する
	bool invalidationFlag = mLandingFlag;

	if (mPushedVector.x)
	{
		if (mFixVector.x)
		{
			invalidationFlag = true;
		}
	}
	else if (mPushedVector.y)
	{
		if (mFixVector.y)
		{
			invalidationFlag = true;
		}
	}
	else if (mPushedVector.z)
	{
		if (mFixVector.z)
		{
			invalidationFlag = true;
		}
	}

	// 無効化
	if (invalidationFlag)
	{
		mFixVector -= mPushedVector;
	}

	static char HitTest = 0;
	SDL_Log("Hit!%d\n", HitTest);
	HitTest ^= 1;
}

void Player::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	if (caller->GetColliderAttribute() == ColliderAttribute::ColAtt_Detector)
	{
		if (mPushedVector.z > 0)
		{
			mLandingFlag = true;

			mDetectGroundFlag = true;

			SetAffectGravityFlag(false);
		}
		return;
	}
}

void Player::OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	static char apartTest = 0;
	SDL_Log("Apart!%d\n", apartTest);
	apartTest ^= 1;
}
