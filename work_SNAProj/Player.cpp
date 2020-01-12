#include "Player.h"
#include "Common.h"
#include "CompletionMeshActor.h"
#include "MeshComponent.h"
#include "BoxColliderComponent.h"
#include "InputMoveComponent.h"
#include "JumpComponent.h"
#include "ClampSpeedComponent.h"
#include "AnimationChips.h"
#include "Mesh.h"
#include "System.h"
#include "Renderer.h"
#include "Input.h"

Player::Player():
	Actor(),
	mLandingFlag(false),
	mDetectGroundFlag(false),
	mLookRightFlag(true),
	mGroundChecker(nullptr),
	mAttackCollider(nullptr),
	mCurrentAnimation(AnimationPattern::Anim_Stay)
{
	// メッシュのロード
	const int drawOrder = 300;
	const float dashAttackAnimSpeed = 1.0f;
	MeshComponent * mc = new MeshComponent(this, drawOrder);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	mMesh->LoadDivTexture("Assets/NinjaStay.png", System::GetInstance().GetRenderer(), this,
		10, 10, 1, 128, 128, 0.07f, AnimationPattern::Anim_Stay);
	mMesh->LoadDivTexture("Assets/NinjaDashAttack02.png", System::GetInstance().GetRenderer(), this,
		8, 8, 1, 128, 128, dashAttackAnimSpeed, AnimationPattern::Anim_DashAttack);
	mc->SetMesh(mMesh);

	// チップからはみ出た部分を描画するためのアクター
	mCompletionMeshActor = new CompletionMeshActor(this, drawOrder);
	mCompletionMeshActor->LoadAnimation("Assets/NinjaDashAttack02_completion.png", System::GetInstance().GetRenderer(),
		8, 8, 1, 128, 128, dashAttackAnimSpeed, AnimationPattern::Anim_DashAttack);
	Mesh * completionMesh = mCompletionMeshActor->GetMesh();
	AABB complMeshBox = completionMesh->GetCollisionBox();
	const float offsetX = (complMeshBox.mMax.x - complMeshBox.mMin.x) * mScale;
	Vector3D cmaPosOffset = Vector3D(offsetX, 0, 0);
	mCompletionMeshActor->SetPositionOffset(cmaPosOffset);
	mCompletionMeshActor->SetFlipDirection(true, false);

	// コライダーの設定
	mBoxCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Player);
	AABB bodyCol = mMesh->GetCollisionBox();
	mBoxCollider->SetObjectBox(bodyCol);

	const bool genDetectorFlag = false;
	if (genDetectorFlag)
	{
		AABB box = mMesh->GetCollisionBox();
		const Vector3D boxSize = box.mMax - box.mMin;
		box.mMax.z -= boxSize.z * 0.8f;
		box.mMin.z -= boxSize.z * 0.2f;

		mGroundChecker = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
		mGroundChecker->SetObjectBox(box);
	}

	AABB attackCol = mMesh->GetCollisionBox();
	float bodyColSizeX = bodyCol.mMax.x - bodyCol.mMin.x;
	attackCol.mMin.x += bodyColSizeX;
	attackCol.mMax.x += bodyColSizeX;
	mAttackCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_PlayerAttack);
	mAttackCollider->SetObjectBox(attackCol);
	mAttackCollider->SetActive(false);
	mAttackCollider->SetMoveOnHitFlag(false);

	mInputComponent = new InputMoveComponent(this);
	mInputComponent->SetVerticalAxis(mInputComponent->AxisEnum_z);

	const float speed = 500.0f;
	mInputComponent->SetHorizontalSpeed(speed);
	mInputComponent->SetVerticalSpeed(-speed);

	// ジャンプ機能
	mJumpComponent = new JumpComponent(this, 10000.0f);

	// 最大速度を調整
	Vector3D limitSpeed(100.0f, 0.0f, 100.0f);
	ClampSpeedComponent * csc = new ClampSpeedComponent(this, limitSpeed);
	csc->SetClampDirectionFlags(true, false, false);

	// 落下スピード割合の調整
	mFallSpeedRate = 2.0f;

	// プレイヤーであることを示すフラグ
	mFlags |= mPlayerFlagMask_Base;
}

Player::~Player()
{
	Common::DeleteContainerOfPointer(mComponentList);

	std::list<const ColliderComponentBase*>().swap(mLandingGrounds);

	SDL_Log("Player is deleted\n");
}

void Player::UpdateActor0()
{
	mDetectGroundFlag = false;

	if (mLandingFlag && Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_SPACE))
	{
		mJumpComponent->Jump();
	}

	// ダッシュアタック
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_A))
	{
		mCurrentAnimation = AnimationPattern::Anim_DashAttack;

		mMesh->SetAnimIndex(this, mCurrentAnimation);
	}

	if (mLandingFlag)
	{
		mLandingFlag = false;
	}
}

void Player::UpdateActor1()
{
	if (mCurrentAnimation == AnimationPattern::Anim_DashAttack && mMesh->GetAnimLoopEndFlag())
	{
		if (!(mCompletionMeshActor->GetMesh()->GetAnimLoopEndFlag()))
		{
			SDL_Log("Completion has not finish\n");
		}

		mMesh->GetActiveAnimChips(this)->Reset();
		mCompletionMeshActor->ResetAnimation(AnimationPattern::Anim_DashAttack);

		mCurrentAnimation = AnimationPattern::Anim_Stay;

		mMesh->SetAnimIndex(this, mCurrentAnimation);
	}

	// チップ補完アクターの設置方向を再設定
	if (mScale != mCompletionMeshActor->GetScale())
	{
		Mesh * completionMesh = mCompletionMeshActor->GetMesh();
		AABB complMeshBox = completionMesh->GetCollisionBox();
		float offsetX = (complMeshBox.mMax.x - complMeshBox.mMin.x) * mScale;
		Vector3D cmaPosOffset = Vector3D(offsetX, 0, 0);
		mCompletionMeshActor->SetPositionOffset(cmaPosOffset);

		if (!mLookRightFlag)
		{
			mCompletionMeshActor->FlipPositionOffset();
		}
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

	// 向き変更
	if (mInputComponent->GetRightKey())
	{
		if (!mLookRightFlag)
		{
			mRotationAngle = 0.0f;

			mRotation = Quaternion(mRotationAxis, mRotationAngle);

			mLookRightFlag = true;

			if (mCompletionMeshActor->GetNowFlippingFlag())
			{
				mCompletionMeshActor->FlipPositionOffset();
			}

			mFlags |= mCalculateTransformFlagMask_Base;
		}
	}
	else if (mInputComponent->GetLeftKey())
	{
		if (mLookRightFlag)
		{
			mRotationAngle = static_cast<float>(M_PI);

			mRotation = Quaternion(mRotationAxis, mRotationAngle);

			mLookRightFlag = false;

			if (!mCompletionMeshActor->GetNowFlippingFlag())
			{
				mCompletionMeshActor->FlipPositionOffset();
			}

			mFlags |= mCalculateTransformFlagMask_Base;
		}
	}

	// 奥行きの情報を常に0に
	mPosition.y = 0.0f;

	// 現在の高さをデバッグ表示
	SDL_Log("Height : %lf", mPosition.z + mMoveVector.z);

	// チップ補完アクターにも現在のアニメーションを伝える
	mCompletionMeshActor->SetAnimationIndex(mCurrentAnimation);
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

	if (mPushedVector.z > 0.0f)
	{
		OnLanding();
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

	if (mPushedVector.z > 0.0f)
	{
		OnLanding();
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

void Player::OnLanding()
{
	mLandingFlag = true;
}
