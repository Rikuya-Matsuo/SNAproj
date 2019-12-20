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
	mGroundChecker = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mGroundChecker->SetObjectBox(box);

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

	std::list<const ColliderComponentBase*>().swap(mLandingGrounds);

	SDL_Log("Player is deleted\n");
}

void Player::UpdateActor0()
{
	if (!mDetectGroundFlag)
	{
		mLandingFlag = false;

		static char testLand = 0;
		SDL_Log("Player : No Landing%d", testLand);
		testLand ^= 1;

		SetAffectGravityFlag(true);
	}

	mDetectGroundFlag = false;
}

void Player::UpdateActor1()
{
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
			OnDetectGround(opponent);

			//SDL_Log("Player : Detect Ground.\n");
		}
		return;
	}

	// すでにその方向への押し返しが働いている場合は、押し返しを無効化する
	// 着地中ならば常に押し返しを無効化する
	bool invalidationFlag = false;

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
	//SDL_Log("Hit!%d\n", HitTest);
	HitTest ^= 1;
}

void Player::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	if (caller->GetColliderAttribute() == ColliderAttribute::ColAtt_Detector)
	{
		if (mPushedVector.z > 0)
		{
			OnDetectGround(opponent);
		}
		return;
	}
}

void Player::OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	if (caller->GetColliderAttribute() == ColliderAttribute::ColAtt_Detector &&
		opponent->GetColliderAttribute() == ColliderAttribute::ColAtt_Block)
	{
		auto itr = std::find(mLandingGrounds.begin(), mLandingGrounds.end(), opponent);

		if (itr != mLandingGrounds.end())
		{
			mLandingGrounds.erase(itr);

			//SDL_Log("Player : Apart Ground.\n");
		}
	}

	static char apartTest = 0;
	//SDL_Log("Apart!%d\n", apartTest);
	apartTest ^= 1;
}

void Player::OnDetectGround(const ColliderComponentBase * opponent)
{
	if (opponent->GetColliderAttribute() == ColliderAttribute::ColAtt_Block)
	{
		// 踏んでいるブロックのコリジョンを記録
		auto itr = std::find(mLandingGrounds.begin(), mLandingGrounds.end(), opponent);

		const bool sucessFind = (itr == mLandingGrounds.end());

		if (!sucessFind)
		{
			mLandingGrounds.emplace_back(opponent);
		}

		// 埋まっている分押し上げる
		float overlap = opponent->GetWorldBox()->mMax.z - mBoxCollider->GetWorldBox()->mMin.z;

		if (overlap > 0)
		{
			mPosition.z += overlap;

			// 行列を更新
			CalculateWorldTransform();

			mGroundChecker->Update();

			// プレイヤーの位置が変わったため、接触コライダーを更新
			mBoxCollider->Update();

			SDL_Log("Player : Detect Ground.\n");
		}
	}

	mLandingFlag = true;

	mDetectGroundFlag = true;

	SetAffectGravityFlag(false);
}
