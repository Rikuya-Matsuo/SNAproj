﻿#include "EnemyTest.h"
#include "Mesh.h"
#include "System.h"
#include "AnimationChips.h"
#include "MeshComponent.h"
#include "AutoMoveComponent.h"
#include "BoxColliderComponent.h"
#include "ClampSpeedComponent.h"
#include "Input.h"

const EnemyTest::FlagType EnemyTest::mDamageAnimFlagMask = 1 << 0;
const EnemyTest::FlagType EnemyTest::mLDetectGroundFlagMask = 1 << 1;
const EnemyTest::FlagType EnemyTest::mRDetectGroundFlagMask = 1 << 2;
const EnemyTest::FlagType EnemyTest::mHitWallFlagMask = 1 << 3;
const EnemyTest::FlagType EnemyTest::mDetectPlayerFlagMask = 1 << 4;
const EnemyTest::FlagType EnemyTest::mDetectWallFlagMask = 1 << 5;
const EnemyTest::FlagType EnemyTest::mTackleFlagMask = 1 << 6;

const float EnemyTest::mTackleWait = 0.8f;


EnemyTest::EnemyTest():
	EnemyBase(3),
	mFlags_EnemyTest(0),
	mTextureIndex(0),
	mDamageAnimTimer(0.0f),
	mTackleWaitTimer(0.0f),
	mPlayerDirection(Vector3D::zero),
	mWallDerection(Vector3D::zero),
	mVelocity(Vector3D(-10.0f, 0.0f, 0.0f)),
	mTackleVelocity(Vector3D(-20.0f, 0.0f, 0.0f)),
	mNormalVelocityLimit(Vector3D(30.0f, 0.0f, 30.0f)),
	mTackleVelocityLimit(Vector3D(70.0f, 0.0f, 50.0f))
{
	mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(), this, 3, 3, 1, 32, 32, 0.0f, 0);
	MeshComponent * mc = new MeshComponent(this, 350);
	mc->SetMesh(mMesh);
	mAnimChips = mMesh->GetAnimChips(this, 0);
	mAnimChips->StopPlaying();

	mAutoMoveComp = new AutoMoveComponent(this, mVelocity);
	mAutoMoveComp->SetReverseFlag(true, false, false);

	mClamper = new ClampSpeedComponent(this, mNormalVelocityLimit);
	mClamper->SetClampDirectionFlags(true, false, false);

	const AABB originalBox = mMesh->GetCollisionBox();;
	AABB colBox = mMesh->GetCollisionBox();
	colBox.mMin.y -= EnemyBase::mDepth;
	colBox.mMax.y += EnemyBase::mDepth;
	mBodyCollision = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	mBodyCollision->SetObjectBox(colBox);

	const Vector3D boxSize = originalBox.mMax - originalBox.mMin;

	// 左側の地面検出装置
	AABB detectorL = originalBox;
	detectorL.mMax.z = detectorL.mMin.z;
	detectorL.mMin.z -= 0.1f;
	detectorL.mMax.x = detectorL.mMin.x;
	detectorL.mMin.x -= boxSize.x / 3;
	mLGroundDetector = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mLGroundDetector->SetObjectBox(detectorL);
	mLGroundDetector->SetRotatableFlag(false);

	// 右側の地面検出装置
	AABB detectorR = originalBox;
	detectorR.mMax.z = detectorR.mMin.z;
	detectorR.mMin.z -= 0.1f;
	detectorR.mMin.x = detectorR.mMax.x;
	detectorR.mMax.x += boxSize.x / 3;
	mRGroundDetector = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mRGroundDetector->SetObjectBox(detectorR);
	mRGroundDetector->SetRotatableFlag(false);

	// 進行方向の壁検出装置
	AABB wallDetector = originalBox;
	wallDetector.mMin.z += boxSize.z / 5;
	wallDetector.mMax.z -= boxSize.z / 5;
	wallDetector.mMax.x = wallDetector.mMin.x;
	wallDetector.mMin.x -= 0.1f;
	mWallDetector = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mWallDetector->SetObjectBox(wallDetector);

	// プレイヤー検出装置
	AABB playerDetector = originalBox;
	playerDetector.mMax.x = playerDetector.mMin.x;
	playerDetector.mMin.x -= boxSize.x * 2.5f;
	playerDetector.mMin.z += boxSize.z / 5;
	playerDetector.mMin.y -= EnemyBase::mDepth;
	playerDetector.mMax.y += EnemyBase::mDepth;
	mPlayerDetector = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mPlayerDetector->SetObjectBox(playerDetector);

	mPrevFlags_EnemyTest = mFlags_EnemyTest;
}

EnemyTest::~EnemyTest()
{
}

void EnemyTest::UpdateEnemy0()
{
	if (mFlags_EnemyTest & mDamageAnimFlagMask)
	{
		mTextureIndex++;

		if (mTextureIndex >= 2)
		{
			mDamageAnimTimer += System::GetInstance().GetDeltaTime();
			const float damAniTime = 1.0f;

			if (mDamageAnimTimer >= damAniTime)
			{
				mDamageAnimTimer = 0.0f;
				mTextureIndex = 0;

				mFlags_EnemyTest &= ~mDamageAnimFlagMask;
			}
			else
			{
				mTextureIndex = 2;
			}
		}

		mAnimChips->SetTextureIndex(mTextureIndex);
	}

	const FlagType detectFlags = mFlags_EnemyTest & (mLDetectGroundFlagMask | mRDetectGroundFlagMask);
	switch (detectFlags)
	{
	// 左だけ検知
	case mLDetectGroundFlagMask:
		if (mFlags_Enemy & mLookRightFlagMask_EBase)
		{
			mFlags_Enemy &= ~mLookRightFlagMask_EBase;
			Flip();
			mAutoMoveComp->ReverseVelocity();
		}
		break;

	// 右だけ検知
	case mRDetectGroundFlagMask:
		if (!(mFlags_Enemy & mLookRightFlagMask_EBase))
		{
			mFlags_Enemy |= mLookRightFlagMask_EBase;
			Flip();
			mAutoMoveComp->ReverseVelocity();
		}
		break;

	// どちらも検出できなかった
	case 0:
		// 空中では移動を行わない
		if (mAutoMoveComp->GetActiveFlag())
		{
			mAutoMoveComp->SetActive(false);
		}
		if (!GetAffectGravityFlag())
		{
			SetAffectGravityFlag(true);
		}
		break;

	// 両方検出
	default:
		if (!mAutoMoveComp->GetActiveFlag())
		{
			mAutoMoveComp->SetActive(true);
		}
		break;
	}

	// 地面にいる間は重力の影響を受けない
	if (detectFlags != 0)
	{
		SetAffectGravityFlag(false);
	}

	// 壁を検出したら反転
	if (mFlags_EnemyTest & mHitWallFlagMask)
	{
		if (!(mPrevFlags_EnemyTest & mHitWallFlagMask))
		{
			bool lookRight = mFlags_Enemy & mLookRightFlagMask_EBase;
			BitFlagFunc::SetFlagByBool(!lookRight, mFlags_Enemy, mLookRightFlagMask_EBase);
			Flip();
			mAutoMoveComp->ReverseVelocity();
		}
	}

	// プレイヤー検知時にタックル準備処理
	if (mFlags_EnemyTest & mDetectPlayerFlagMask)
	{
		TackleProcess();
	}
	else
	{
		// 着地している　&&　自動移動が非アクティブ
		if (detectFlags && !mAutoMoveComp->GetActiveFlag())
		{
			// 自動移動をアクティブに
			mAutoMoveComp->SetActive(true);
		}

		// タックルしていたなら
		if (mFlags_EnemyTest & mTackleFlagMask)
		{
			mAutoMoveComp->SetVelocity(mVelocity);
			if (mFlags_Enemy & mLookRightFlagMask_EBase)
			{
				mAutoMoveComp->ReverseVelocity();
			}

			mClamper->SetLimit(mNormalVelocityLimit);

			mFlags_EnemyTest &= ~mTackleFlagMask;
		}

		mTackleWaitTimer = 0.0f;
	}

	// 自動移動が非アクティブなら水平方向移動ベクトルを0に
	if (!mAutoMoveComp->GetActiveFlag())
	{
		mMoveVector.x = 0.0f;
	}
}

void EnemyTest::TackleProcess()
{
	if (mTackleWaitTimer < mTackleWait)
	{
		mTackleWaitTimer += System::GetInstance().GetDeltaTime();
		mAutoMoveComp->SetActive(false);
	}
	else
	{
		// 突進開始
		mFlags_EnemyTest |= mTackleFlagMask;

		if (!(mPrevFlags_EnemyTest & mTackleFlagMask))
		{
			mAutoMoveComp->SetActive(true);
			float f = (mPlayerDirection.x > 0.0f) ? -1.0f : 1.0f;
			mAutoMoveComp->SetVelocity(mTackleVelocity * f);

			mClamper->SetLimit(mTackleVelocityLimit);
		}
	}
}

void EnemyTest::UpdateEnemy1()
{
	mPrevFlags_EnemyTest = mFlags_EnemyTest;

	EnemyTest::FlagType mask =
		(mLDetectGroundFlagMask | mRDetectGroundFlagMask | mHitWallFlagMask | mDetectPlayerFlagMask | mDetectWallFlagMask);
	mFlags_EnemyTest &= ~mask;
}

void EnemyTest::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	Uint8 callerAtt = caller->GetColliderAttribute();

	if (caller == mBodyCollision && opponentAtt == ColliderAttribute::ColAtt_PlayerAttack)
	{
		mFlags_EnemyTest |= mDamageAnimFlagMask;
	}

	// 地面検出装置の処理
	auto checkPointer = 
		[this, caller, opponentAtt](const BoxColliderComponent * detector, const FlagType mask, ColliderAttribute cmpAtt = ColliderAttribute::ColAtt_Block)
	{
		bool ret = false;

		if (detector == caller && opponentAtt == cmpAtt)
		{
			if (cmpAtt == ColAtt_Player)
			{
				SDL_Delay(0);
			}

			mFlags_EnemyTest |= mask;

			ret = true;
		}

		return ret;
	};

	checkPointer(mLGroundDetector, mLDetectGroundFlagMask);
	checkPointer(mRGroundDetector, mRDetectGroundFlagMask);
	checkPointer(mWallDetector, mHitWallFlagMask);

	bool detPlayer = checkPointer(mPlayerDetector, mDetectPlayerFlagMask, ColliderAttribute::ColAtt_Player);
	if (detPlayer)
	{
		mPlayerDirection = opponent->GetOwner()->GetPosition() - mPosition;

#ifdef DEBUG_SNA

		SDL_Log("detect Player.%d\n", System::frameCount);

#endif // DEBUG_SNA
	}

	bool detBlock = checkPointer(mPlayerDetector, mDetectWallFlagMask);
	if (detBlock)
	{
		mWallDerection = opponent->GetOwner()->GetPosition() - mPosition;
	}
}

void EnemyTest::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	Uint8 callerAtt = caller->GetColliderAttribute();

	// 地面検出装置の処理
	auto checkPointer = 
		[this, caller, opponentAtt](const BoxColliderComponent * detector, const FlagType mask, ColliderAttribute cmpAtt = ColliderAttribute::ColAtt_Block)
	{
		bool ret = false;

		if (detector == caller && opponentAtt == cmpAtt)
		{
			mFlags_EnemyTest |= mask;

			ret = true;
		}

		return ret;
	};

	checkPointer(mLGroundDetector, mLDetectGroundFlagMask);
	checkPointer(mRGroundDetector, mRDetectGroundFlagMask);
	checkPointer(mWallDetector, mHitWallFlagMask);

	bool detPlayer = checkPointer(mPlayerDetector, mDetectPlayerFlagMask, ColliderAttribute::ColAtt_Player);
	if (detPlayer)
	{
		mPlayerDirection = opponent->GetOwner()->GetPosition() - mPosition;

#ifdef DEBUG_SNA

		SDL_Log("detect Player.%d\n", System::frameCount);

#endif // DEBUG_SNA
	}

	bool detBlock = checkPointer(mPlayerDetector, mDetectWallFlagMask);
	if (detBlock)
	{
		mWallDerection = opponent->GetOwner()->GetPosition() - mPosition;
	}
}
