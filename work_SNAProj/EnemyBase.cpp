#include "EnemyBase.h"
#include "MeshComponent.h"
#include "System.h"
#include "BlockHitChecker.h"

const EnemyBase::FlagType EnemyBase::mAliveFlagMask_EBase = 1 << 0;
const EnemyBase::FlagType EnemyBase::mImmortalFlagMask_EBase = 1 << 1;
const EnemyBase::FlagType EnemyBase::mLookRightFlagMask_EBase = 1 << 2;
const EnemyBase::FlagType EnemyBase::mAttackFlagMask_EBase = 1 << 3;

const float EnemyBase::mDepth = 0.05f;

EnemyBase::EnemyBase(unsigned char lifeMax) :
	mLifeMax(lifeMax),
	mLife(lifeMax),
	mFlags_Enemy(mAliveFlagMask_EBase)
{
	SetPriority(100);

	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);

	//mFlags_Enemy |= mImmortalFlagMask_EBase;
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::OnLifeRunOut()
{
	// 生存フラグを下す
	mFlags_Enemy &= ~mAliveFlagMask_EBase;
}

void EnemyBase::Flip()
{
	mRotationAngle = (mRotationAngle == 0.0f) ? static_cast<float>(M_PI) : 0.0f;
	OnFlip();
}

void EnemyBase::OnFlip()
{
}

void EnemyBase::UpdateActor0()
{
	if (mLife <= 0 && !(mFlags_Enemy & mImmortalFlagMask_EBase))
	{
		OnLifeRunOut();
	}

	if (!(mFlags_Enemy & mAliveFlagMask_EBase))
	{
		mFlags |= mStopUpdateFlagMask_Base;
	}

	UpdateEnemy0();
}

void EnemyBase::UpdateActor1()
{
	UpdateEnemy1();

	if (mPosition.y != mDepth)
	{
		mPosition.y = mDepth;
		mFlags |= mCalculateTransformFlagMask_Base;
	}
}
