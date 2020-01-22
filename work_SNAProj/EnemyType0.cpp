#include "EnemyType0.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "System.h"
#include "Effect.h"
#include "BlockHitChecker.h"
#include "BoxColliderComponent.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "AnimationChips.h"

const float EnemyType0::mTackleWait = 0.8f;

const EnemyType0::FlagType EnemyType0::mDamageAnimFlagMask = 1 << 0;
const EnemyType0::FlagType EnemyType0::mFindPlayerFlagMask = 1 << 1;
const EnemyType0::FlagType EnemyType0::mTackleFlagMask = 1 << 2;

using namespace BlockHitDirectionFlagMask;

EnemyType0::EnemyType0():
	mNormalLimit(Vector3D(30.0f, 0.0f, 30.0f)),
	mTackleLimit(Vector3D(70.0f, 0.0f, 50.0f)),
	mKnockBackLimit(Vector3D(50.0f, 0.0f, 50.0f)),
	mVelocity(Vector3D(-10.0f, 0.0f, 0.0f)),
	mTackleVelocity(Vector3D(-20.0f, 0.0f, 0.0f)),
	mKnockBackVector(Vector3D(10.0f, 0.0, 10.0f)),
	mTackleWaitTimer(0.0f),
	mFlags_Enemy0(0)
{
	mMesh->LoadDivTexture("Assets/AnimChipTest.png", System::GetInstance().GetRenderer(), this, 3, 3, 1, 32, 32, 0.0f, 0);
	MeshComponent * mc = new MeshComponent(this, 350);
	mc->SetMesh(mMesh);
	mAnimChips = mMesh->GetAnimChips(this, 0);
	mAnimChips->StopPlaying();

	const AABB original = mMesh->GetCollisionBox();
	Vector3D size = original.mMax - original.mMin;

	AABB body = original;
	body.mMax.y += EnemyBase::mDepth;
	body.mMin.y -= EnemyBase::mDepth;
	mBodyCollision = new BoxColliderComponent(this, ColAtt_Enemy);
	mBodyCollision->SetObjectBox(body);

	AABB detector = mMesh->GetCollisionBox();
	detector.mMax.x = detector.mMin.x;
	detector.mMin.x -= size.x * 2.5f;
	detector.mMax.z -= size.z / 5;
	detector.mMin.z += size.z / 5;
	mPlayerDetector = new BoxColliderComponent(this, ColAtt_Detector);
	mPlayerDetector->SetObjectBox(detector);

	mBlockChecker = new BlockHitChecker(this, mBodyCollision);

	mFindPlayerEffect = new Effect("Assets/findPlayerEff.png", mPriority + 50);
	mFindPlayerEffect->SetAppearSecond(mTackleWait);

	mAutoMoveComp = new AutoMoveComponent(this, mVelocity);
	mAutoMoveComp->SetReverseFlag(true, false, false);

	mClamper = new ClampSpeedComponent(this, mNormalLimit);

	SetScale(25.0f);

	mPrevFlag_Enemy0 = mFlags_Enemy0;
}

EnemyType0::~EnemyType0()
{
}

void EnemyType0::UpdateEnemy0()
{
}

void EnemyType0::UpdateEnemy1()
{
	const Uint8 a = mBlockChecker->GetFlags();
	if (a & mDownMask)
	{
		SetAffectGravityFlag(false);
	}

	if (mFlags_Enemy & mLookRightFlagMask_EBase)
	{
		if (a & mRightMask)
		{
			Flip();
		}
	}
	else if (a & mLeftMask)
	{
		Flip();
	}

	mPrevFlag_Enemy0 = mFlags_Enemy0;
}

void EnemyType0::OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	DetectPlayer(caller, opponentAtt);

	OnPressedByplayer(caller, opponentAtt);
}

void EnemyType0::OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent)
{
	Uint8 opponentAtt = opponent->GetColliderAttribute();
	DetectPlayer(caller, opponentAtt);

	OnPressedByplayer(caller, opponentAtt);
}

void EnemyType0::OnFlip()
{
	bool lookRight = mFlags_Enemy & mLookRightFlagMask_EBase;
	BitFlagFunc::SetFlagByBool(!lookRight, mFlags_Enemy, mLookRightFlagMask_EBase);
	mAutoMoveComp->ReverseVelocity();
}

void EnemyType0::OnPressedByplayer(const ColliderComponentBase * caller, Uint8 opponentAtt)
{
	if (caller == mBodyCollision && opponentAtt == ColAtt_Player && mPushedVector.z < 0)
	{
		mFixVector.z -= mPushedVector.z;
		mPushedVector.z = 0;
	}
}

void EnemyType0::DetectPlayer(const ColliderComponentBase * caller, Uint8 opponentAtt)
{
	if (caller == mPlayerDetector && opponentAtt == ColliderAttribute::ColAtt_Player)
	{
		mFlags_Enemy0 |= mFindPlayerFlagMask;
	}


}
