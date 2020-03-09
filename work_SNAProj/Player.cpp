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
#include "AnimationEffect.h"
#include "NinjaArtsBase.h"
#include "BlockHitChecker.h"

#ifdef DEBUG_SNA
static bool debugFlag = false;
#endif // DEBUG_SNA

using namespace BlockHitDirectionFlagMask;

const char Player::mLifeMax = 10;
const char Player::mDashAttackPower = 1;

const Player::FlagType Player::mLandingFlagMask			= 1 << 0;
const Player::FlagType Player::mDetectGroundFlagMask	= 1 << 1;
const Player::FlagType Player::mLookRightFlagMask		= 1 << 2;
const Player::FlagType Player::mImmortalFlagMask		= 1 << 3;
const Player::FlagType Player::mAliveFlagMask			= 1 << 4;
const Player::FlagType Player::mKnockBackFlagMask		= 1 << 5;
const Player::FlagType Player::mJumpInputFlagMask		= 1 << 6;

Player::Player() :
	Actor(),
	mFlags_Player(mLookRightFlagMask | mAliveFlagMask),
	mGroundChecker(nullptr),
	mAttackCollider(nullptr),
	mCurrentCursorNinjaArts(nullptr),
	mCurrentAnimation(AnimationPattern::Anim_Stay),
	mLife(mLifeMax),
	mHitEffectMass(4)
{
	// フラグコピー
	mPrevFlags_Player = mFlags_Player;

	// メッシュのロード
	const int drawOrder = 300;
	const float dashAttackAnimSpeed = 0.05f;
	MeshComponent * mc = new MeshComponent(this, drawOrder);
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
	// 待機アニメーション
	mMesh->LoadDivTexture("Assets/NinjaStay.png", System::GetInstance().GetRenderer(), this,
		10, 10, 1, 128, 128, 0.07f, AnimationPattern::Anim_Stay);
	// 攻撃アニメーション
	mMesh->LoadDivTexture("Assets/NinjaDashAttack02.png", System::GetInstance().GetRenderer(), this,
		8, 8, 1, 128, 128, dashAttackAnimSpeed, AnimationPattern::Anim_DashAttack);
	// 走りアニメーション
	mMesh->LoadDivTexture("Assets/NinjaRun02.png", System::GetInstance().GetRenderer(), this,
		5, 5, 1, 128, 128, 0.05f, AnimationPattern::Anim_Run);
	AnimationChips * runChips = mMesh->GetAnimChips(this, Anim_Run);
	int routine[] = { 0,1,2,3,-1 };
	runChips->SetRoutine(routine);
	// ノックバックアニメーション
	mMesh->LoadDivTexture("Assets/NinjaDown.png", System::GetInstance().GetRenderer(), this,
		6, 6, 1, 128, 128, 0.06f, AnimationPattern::Anim_KnockBack);
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

	// 二次元的当たり判定コンポーネント
	mHitChecker = new BlockHitChecker(this, mBoxCollider);
	mHitChecker->SetOnHitDownFunction(OnHitDown);

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
	mJumpComponent->SetPower(9.0f);

	// 最大速度を調整
	Vector3D limitSpeed(170.0f, 0.0f, 100.0f);
	ClampSpeedComponent * csc = new ClampSpeedComponent(this, limitSpeed);
	csc->SetClampDirectionFlags(true, false, false);

	// 攻撃ヒットエフェクトの配列
	mHitEffects = new AnimationEffect*[mHitEffectMass];
	for (size_t i = 0; i < mHitEffectMass; ++i)
	{
		mHitEffects[i] = new AnimationEffect(mPriority + 50,
			"Assets/hitEff1.png", 4, 2, 2, 256, 256, 0.09f);
		mHitEffects[i]->SetScale(50.0f);
	}

	// 落下スピード割合の調整
	mFallSpeedRate = 25.0f;

	// 落下スピードの制限値設定
	mFallSpeedMax = 6.0f;
}

Player::~Player()
{
	// インスタンス自体はActorクラスを継承しているため、Systemクラスによってメモリ解放が行われる
	delete[] mHitEffects;
	mHitEffects = nullptr;

	std::list<EnemyBase *>().swap(mHitList);

	SDL_Log("Player is deleted\n");
}

void Player::UpdateActor0()
{
#ifdef DEBUG_SNA
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_LCTRL))
	{
		debugFlag = !debugFlag;
	}
#endif // DEBUG_SNA

	// 落下時は死亡とする
	if (mPosition.z < -50.0f)
	{
		mLife = 0;
	}

	// ライフが０以下、かつ不死身フラグが立っていない場合
	if (mLife <= 0 && !(mFlags_Player & mImmortalFlagMask))
	{
		OnLifeRunOut();
	}

	if (!(mFlags_Player & mAliveFlagMask) && mPrevFlags_Player & mAliveFlagMask)
	{
		SetAllComponentActive(false);
	}

	// ジャンプ操作受付
	mFlags_Player &= ~mJumpInputFlagMask;
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_SPACE) || Input::GetInstance().GetGamePadButtonPressDown(SDL_CONTROLLER_BUTTON_A))
	{
		mFlags_Player |= mJumpInputFlagMask;
	}
#ifdef DEBUG_SNA
	if (debugFlag && mFlags_Player & mLandingFlagMask)
	{
		static char test = 0;
		SDL_Log("Landing.%d\n", test);
		if (++test > 100)
		{
			test = 0;
		}
	}
#endif
	// ジャンプ実行
	if (mFlags_Player & mLandingFlagMask && mFlags_Player & mJumpInputFlagMask)
	{
		mJumpComponent->Jump();
	}

	// ダッシュアタック
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_A) || Input::GetInstance().GetGamePadButtonPressDown(SDL_CONTROLLER_BUTTON_B))
	{
		mCurrentAnimation = AnimationPattern::Anim_DashAttack;

		mAttackCollider->SetActive(true);
	}

	// 制作展では使わない
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_N))
	{
		if (mCurrentCursorNinjaArts)
		{
			mCurrentCursorNinjaArts->Use();
		}
	}

#ifdef DEBUG_SNA
	if (Input::GetInstance().GetKeyPressDown(SDL_SCANCODE_LCTRL))
	{
		SDL_Log("height : %lf", mPosition.z);
	}
#endif // DEBUG_SNA
}

void Player::UpdateActor1()
{
	// 二次元的当たり判定の結果を見て処理
	Uint8 hitDir = mHitChecker->GetHitDirectionFlags();

	// フラグリセット
	mFlags_Player &= ~mLandingFlagMask;

	// 床と接触した場合
	if (hitDir & mDownMask)
	{
#ifdef DEBUG_SNA
		/*
		static char test = 0;
		SDL_Log("Down Hit.%d\n", test);
		if (++test > 100)
		{
			test = 0;
		}
		*/
#endif

		OnDetectGround();

		if (mMoveVector.z < 0.0f)
		{
			mMoveVector.z = 0.0f;
		}
	}

	// 着地していない場合
	if (!(mFlags_Player & mLandingFlagMask))
	{
		// 重力有効化
		SetAffectGravityFlag(true);

		// 着地フラグを切る
		mFlags_Player &= ~mLandingFlagMask;
	}

	// 走る
	if (mInputComponent->GetHorizonInputFlag())
	{
		if (mCurrentAnimation != Anim_DashAttack)
		{
			mCurrentAnimation = AnimationPattern::Anim_Run;
		}
	}
	else if (mCurrentAnimation != Anim_DashAttack)
	{
		mCurrentAnimation = Anim_Stay;
	}

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

	// ノックバックアニメーション
	AnimationChips * knockBackAnim = mMesh->GetAnimChips(this, Anim_KnockBack);
	if (mFlags_Player & mKnockBackFlagMask)
	{
		mCurrentAnimation = AnimationPattern::Anim_KnockBack;

		if (knockBackAnim->GetLoopEndFlag())
		{
			knockBackAnim->StopPlaying();

			knockBackAnim->SetTextureIndex(5);
		}
	}
	else
	{
		knockBackAnim->Reset();
		knockBackAnim->StartPlaying();
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
	if (!mInputComponent->GetHorizonInputFlag() && !(mFlags_Player & mKnockBackFlagMask))
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

	// メッシュにアニメーションの変更を伝える
	mMesh->SetAnimIndex(this, mCurrentAnimation);

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

	if (callerAtt == ColliderAttribute::ColAtt_Detector && (opponentAtt == ColliderAttribute::ColAtt_Block || opponentAtt == ColliderAttribute::ColAtt_Enemy))
	{
		OnDetectGround();

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

	if (callerAtt == ColAtt_Player && opponentAtt == ColAtt_Enemy)
	{
		EnemyBase * enemy = static_cast<EnemyBase*>(opponent->GetOwner());
		if (enemy->GetAttackFlag())
		{
			float x = enemy->GetPosition().x - mPosition.x;
			mMoveVector = Vector3D(20.0f, 0.0f, 8.0f);
			mMoveVector.x *= x < 0.0f ? 1.0f : -1.0f;

			mFlags_Player |= mKnockBackFlagMask;

			Effect * eff = FindNonActiveEffect(mHitEffects, mHitEffectMass);
			if (eff)
			{
				eff->SetPosition(mPosition);
				eff->SetActive(true);
			}
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

			Effect * eff = FindNonActiveEffect(mHitEffects, mHitEffectMass);
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
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	if (caller->GetColliderAttribute() == ColliderAttribute::ColAtt_Detector && (opponentAtt == ColliderAttribute::ColAtt_Block || opponentAtt == ColliderAttribute::ColAtt_Enemy))
	{
		OnDetectGround();

		return;
	}

	if (mPushedVector.z > 0.0f)
	{
		OnLanding();
	}
}

void Player::OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
}

void Player::OnDetectGround()
{
	mFlags_Player |= mLandingFlagMask;

	mFlags_Player |= mDetectGroundFlagMask;

	if ((mFlags_Player & mPrevFlags_Player & mKnockBackFlagMask) && mPushedVector.z > 0.0f)
	{
		mFlags_Player &= ~mKnockBackFlagMask;
	}

	SetAffectGravityFlag(false);
}

void Player::OnLanding()
{
	mFlags_Player |= mLandingFlagMask;
}

void Player::OnLifeRunOut()
{
	mFlags_Player &= ~mAliveFlagMask;
}

void Player::OnHitDown(Actor * actor)
{
	Player * p = static_cast<Player*>(actor);

	p->OnDetectGround();
}

AnimationEffect * Player::FindNonActiveEffect(AnimationEffect ** effArray, size_t mass) const
{
	AnimationEffect * ret = nullptr;

	for (size_t i = 0; i < mass; ++i)
	{
		if (!effArray[i]->GetActiveFlag())
		{
			ret = effArray[i];
			break;
		}
	}

	return ret;
}
