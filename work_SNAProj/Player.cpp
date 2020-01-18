﻿#include "Player.h"
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
#include "EnemyBase.h"
#include "Effect.h"
#include "NinjaArtsBase.h"

const char Player::mLifeMax = 10;
const char Player::mDashAttackPower = 1;

const Player::FlagType Player::mLandingFlagMask			= 1 << 0;
const Player::FlagType Player::mDetectGroundFlagMask	= 1 << 1;
const Player::FlagType Player::mLookRightFlagMask		= 1 << 2;
const Player::FlagType Player::mImmortalFlagMask		= 1 << 3;
const Player::FlagType Player::mAliveFlagMask			= 1 << 4;

Player::Player() :
	Actor(),
	mFlags_Player(mLookRightFlagMask | mAliveFlagMask),
	mGroundChecker(nullptr),
	mAttackCollider(nullptr),
	mCurrentCursorNinjaArts(nullptr),
	mCurrentAnimation(AnimationPattern::Anim_Stay),
	mLife(mLifeMax),
	mHitEffectMass(2)
{
	// フラグコピー
	mPrevFlags_Player = mFlags_Player;

	// メッシュのロード
	const int drawOrder = 300;
	const float dashAttackAnimSpeed = 0.05f;
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

	const bool genDetectorFlag = true;
	if (genDetectorFlag)
	{
		AABB box = bodyCol;
		const Vector3D boxSize = bodyCol.mMax - bodyCol.mMin;
		box.mMax.z -= boxSize.z * 0.9f;
		box.mMin.z = bodyCol.mMin.z - 0.1f;

		float detectorXOffset = boxSize.x * 0.95f / 2;
		box.mMax.x -= detectorXOffset;
		box.mMin.x += detectorXOffset;

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
	mAttackCollider->SetPushOnHitFlag(false);

	// 矢印キー移動コンポーネント
	mInputComponent = new InputMoveComponent(this);
	mInputComponent->SetVerticalAxis(mInputComponent->AxisEnum_Invalid);

	const float speed = 500.0f;
	mInputComponent->SetHorizontalSpeed(speed);
	mInputComponent->SetVerticalSpeed(-speed);

	// ジャンプ機能
	mJumpComponent = new JumpComponent(this);
	mJumpComponent->SetPower(6.0f);

	// 最大速度を調整
	Vector3D limitSpeed(170.0f, 0.0f, 100.0f);
	ClampSpeedComponent * csc = new ClampSpeedComponent(this, limitSpeed);
	csc->SetClampDirectionFlags(true, false, false);

	// 攻撃ヒットエフェクトの配列
	mHitEffects = new Effect*[mHitEffectMass];
	for (size_t i = 0; i < mHitEffectMass; ++i)
	{
		mHitEffects[i] = new Effect("Assets/effectKari.png");
		mHitEffects[i]->SetScale(20.0f);
	}

	// 落下スピード割合の調整
	mFallSpeedRate = 25.0f;

	// 落下スピードの制限値設定
	mFallSpeedMax = 50.0f;

	// プレイヤーであることを示すフラグ
	mFlags |= mPlayerFlagMask_Base;
}

Player::~Player()
{
	SDL_Log("Player is deleted\n");
}

void Player::UpdateActor0()
{
	if (mLife <= 0 && !(mFlags_Player & mImmortalFlagMask))
	{
		OnLifeRunOut();
	}

	if (!(mFlags_Player & mAliveFlagMask) && mPrevFlags_Player & mAliveFlagMask)
	{
		SetAllComponentActive(false);
	}

	if (!(mFlags_Player & mDetectGroundFlagMask))
	{
		SetAffectGravityFlag(true);
		mFlags_Player &= ~mLandingFlagMask;
	}
	mFlags_Player &= ~mDetectGroundFlagMask;

	if (mFlags_Player & mLandingFlagMask && Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_SPACE))
	{
		mJumpComponent->Jump();
	}

	// ダッシュアタック
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_A))
	{
		mCurrentAnimation = AnimationPattern::Anim_DashAttack;

		mMesh->SetAnimIndex(this, mCurrentAnimation);

		mAttackCollider->SetActive(true);
	}

	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_N))
	{
		if (mCurrentCursorNinjaArts)
		{
			mCurrentCursorNinjaArts->Use();
		}
	}

	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_LCTRL))
	{
		SDL_Log("height : %lf", mPosition.z);
	}
}

void Player::UpdateActor1()
{
	// ダッシュアタック終了判定
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

		mAttackCollider->SetActive(false);

		mHitList.clear();
	}

	// チップ補完アクターの設置方向を再設定
	if (mScale != mCompletionMeshActor->GetScale())
	{
		Mesh * completionMesh = mCompletionMeshActor->GetMesh();
		AABB complMeshBox = completionMesh->GetCollisionBox();
		float offsetX = (complMeshBox.mMax.x - complMeshBox.mMin.x) * mScale;
		Vector3D cmaPosOffset = Vector3D(offsetX, 0, 0);
		mCompletionMeshActor->SetPositionOffset(cmaPosOffset);

		if (!(mFlags_Player & mLookRightFlagMask))
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
		if (!(mFlags_Player & mLookRightFlagMask))
		{
			mRotationAngle = 0.0f;

			//mRotation = Quaternion(mRotationAxis, mRotationAngle);

			mFlags_Player |= mLookRightFlagMask;

			if (mCompletionMeshActor->GetNowFlippingFlag())
			{
				mCompletionMeshActor->FlipPositionOffset();
			}

			mFlags |= mCalculateTransformFlagMask_Base;
		}
	}
	else if (mInputComponent->GetLeftKey())
	{
		if ((mFlags_Player & mLookRightFlagMask))
		{
			mRotationAngle = static_cast<float>(M_PI);

			//mRotation = Quaternion(mRotationAxis, mRotationAngle);

			mFlags_Player &= ~mLookRightFlagMask;

			if (!mCompletionMeshActor->GetNowFlippingFlag())
			{
				mCompletionMeshActor->FlipPositionOffset();
			}

			mFlags |= mCalculateTransformFlagMask_Base;
		}
	}

	// 奥行きの情報を常に0に
	mPosition.y = 0.0f;

	// チップ補完アクターにも現在のアニメーションを伝える
	mCompletionMeshActor->SetAnimationIndex(mCurrentAnimation);

	// テクスチャ番号を設定
	if (mCompletionMeshActor->IsResisteredIndex(mCurrentAnimation))
	{
		size_t index = mMesh->GetActiveAnimChips(this)->GetCurrentTextureIndex();
		mCompletionMeshActor->GetMesh()->GetAnimChips(mCompletionMeshActor, mCurrentAnimation)->SetTextureIndex(index);
	}

	// フラグ記録
	mPrevFlags_Player = mFlags_Player;
}

void Player::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	ColliderAttribute callerAtt = caller->GetColliderAttribute();
	ColliderAttribute opponentAtt = opponent->GetColliderAttribute();

	if (callerAtt == ColliderAttribute::ColAtt_Detector)
	{
		OnDetectGround(opponent);

		return;
	}

	if (mPushedVector.z > 0.0f)
	{
		OnLanding();
	}

	if (opponentAtt == ColliderAttribute::ColAtt_Block || opponentAtt == ColliderAttribute::ColAtt_Enemy)
	{
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
	}

	if (callerAtt == ColliderAttribute::ColAtt_PlayerAttack && opponentAtt == ColliderAttribute::ColAtt_Enemy)
	{
		EnemyBase * enemy = static_cast<EnemyBase*>(opponent->GetOwner());

		auto itr = std::find(mHitList.begin(), mHitList.end(), enemy);
		if (itr == mHitList.end())
		{
			enemy->Damage(mDashAttackPower);
			mHitList.emplace_back(enemy);

			Effect * eff = FindNonActiveEffect(const_cast<const Effect**>(mHitEffects), mHitEffectMass);
			if (eff)
			{
				eff->SetPosition(enemy->GetPosition());
				eff->SetActive(true);
			}
			else
			{
				SDL_Delay(0);
			}
		}
	}
}

void Player::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	if (caller->GetColliderAttribute() == ColliderAttribute::ColAtt_Detector)
	{
		OnDetectGround(opponent);

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
	}

	static char apartTest = 0;
	//SDL_Log("Apart!%d\n", apartTest);
	apartTest ^= 1;
}

void Player::OnDetectGround(const ColliderComponentBase * opponent)
{
	mFlags_Player |= mLandingFlagMask;

	mFlags_Player |= mDetectGroundFlagMask;

	SetAffectGravityFlag(false);
}

void Player::OnLanding()
{
	mFlags_Player |= mLandingFlagMask;
}

void Player::OnLifeRunOut()
{
}

Effect * Player::FindNonActiveEffect(const Effect ** effArray, size_t mass) const
{
	Effect * ret = nullptr;

	for (size_t i = 0; i < mass; ++i)
	{
		if (!effArray[i]->GetActiveFlag())
		{
			ret = const_cast<Effect*>(effArray[i]);
			break;
		}
	}

	return ret;
}
