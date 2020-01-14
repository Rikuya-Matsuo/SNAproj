#include "EnemyBase.h"
#include "MeshComponent.h"
#include "System.h"

const EnemyBase::FlagType EnemyBase::mAliveFlagMask = 1 << 0;
const EnemyBase::FlagType EnemyBase::mFindPlayerFlagMask = 1 << 1;
const EnemyBase::FlagType EnemyBase::mImmortalFlagMask = 1 << 2;

EnemyBase::EnemyBase(unsigned char lifeMax) :
	mLifeMax(lifeMax),
	mLife(lifeMax),
	mFlags_Enemy(mAliveFlagMask)
{
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::OnLifeRunOut()
{
	// 生存フラグを下す
	mFlags_Enemy &= ~mAliveFlagMask;
}

void EnemyBase::UpdateActor0()
{
	if (mLife <= 0 && !(mFlags_Enemy & mImmortalFlagMask))
	{
		OnLifeRunOut();
	}

	if (!(mFlags_Enemy & mAliveFlagMask))
	{
		mFlags |= mStopUpdateFlagMask_Base;
	}

	UpdateEnemy0();
}

void EnemyBase::UpdateActor1()
{
	UpdateEnemy1();

	mPosition.y = 0.0f;
}
