#include "EnemyTest.h"
#include "Mesh.h"
#include "System.h"
#include "AnimationChips.h"
#include "MeshComponent.h"
#include "AutoMoveComponent.h"
#include "BoxColliderComponent.h"
#include "ClampSpeedComponent.h"

const EnemyTest::FlagType EnemyTest::mDamageAnimFlagMask = 1 << 0;
const EnemyTest::FlagType EnemyTest::mLDetectGroundFlagMask = 1 << 1;
const EnemyTest::FlagType EnemyTest::mRDetectGroundFlagMask = 1 << 2;

EnemyTest::EnemyTest():
	EnemyBase(3),
	mFlags_EnemyTest(0),
	mTextureIndex(0),
	mDamageAnimTimer(0.0f)
{
	mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(), this, 3, 3, 1, 32, 32, 0.0f, 0);
	MeshComponent * mc = new MeshComponent(this, 350);
	mc->SetMesh(mMesh);
	mAnimChips = mMesh->GetAnimChips(this, 0);
	mAnimChips->StopPlaying();

	mVelocity = Vector3D(-10.0f, 0.0f, 0.0f);

	mAutoMoveComp = new AutoMoveComponent(this, mVelocity);
	mAutoMoveComp->SetReverseFlag(true, false, false);

	ClampSpeedComponent * csc = new ClampSpeedComponent(this, Vector3D(30.0f, 0.0f, 30.0f));
	csc->SetClampDirectionFlags(true, false, false);

	const AABB originalBox = mMesh->GetCollisionBox();;
	AABB colBox = mMesh->GetCollisionBox();
	colBox.mMin.y -= EnemyBase::mDepth;
	colBox.mMax.y += EnemyBase::mDepth;
	BoxColliderComponent * bcc = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Enemy);
	bcc->SetObjectBox(colBox);

	const Vector3D boxSize = originalBox.mMax - originalBox.mMin;

	// 左側の地面検出装置
	AABB detectorL = originalBox;
	detectorL.mMax.z = detectorL.mMin.z;
	detectorL.mMin.z -= 0.1f;
	detectorL.mMax.x = detectorL.mMin.x;
	detectorL.mMin.x -= boxSize.x / 3;
	mLDetector = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mLDetector->SetObjectBox(detectorL);
	mLDetector->SetRotatableFlag(false);

	// 右側の地面検出装置
	AABB detectorR = originalBox;
	detectorR.mMax.z = detectorR.mMin.z;
	detectorR.mMin.z -= 0.1f;
	detectorR.mMin.x = detectorR.mMax.x;
	detectorR.mMax.x += boxSize.x / 3;
	mRDetector = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);
	mRDetector->SetObjectBox(detectorR);
	mRDetector->SetRotatableFlag(false);
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
		break;

	// 両方検出
	default:
		if (!mAutoMoveComp->GetActiveFlag())
		{
			mAutoMoveComp->SetActive(true);
		}
		break;
	}

	mFlags_EnemyTest &= ~(mLDetectGroundFlagMask | mRDetectGroundFlagMask);
}

void EnemyTest::UpdateEnemy1()
{
}

void EnemyTest::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	Uint8 callerAtt = caller->GetColliderAttribute();

	if (opponentAtt == ColliderAttribute::ColAtt_PlayerAttack)
	{
		mFlags_EnemyTest |= mDamageAnimFlagMask;

		SDL_Log("Damage!\n");
	}

	// 地面検出装置の処理
	auto checkPointer = [this, caller, opponentAtt](const BoxColliderComponent * detector, const FlagType mask)
	{
		bool ret = false;

		if (detector == caller && opponentAtt == ColliderAttribute::ColAtt_Block)
		{
			mFlags_EnemyTest |= mask;

			ret = true;
		}

		return ret;
	};

	if (!checkPointer(mLDetector, mLDetectGroundFlagMask))
	{
		checkPointer(mRDetector, mRDetectGroundFlagMask);
	}
}

void EnemyTest::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	Uint8 callerAtt = caller->GetColliderAttribute();

	// 地面検出装置の処理
	auto checkPointer = [this, caller, opponentAtt](const BoxColliderComponent * detector, const FlagType mask)
	{
		bool ret = false;

		if (detector == caller && opponentAtt == ColliderAttribute::ColAtt_Block)
		{
			mFlags_EnemyTest |= mask;

			ret = true;
		}

		return ret;
	};

	if (!checkPointer(mLDetector, mLDetectGroundFlagMask))
	{
		checkPointer(mRDetector, mRDetectGroundFlagMask);
	}
}
