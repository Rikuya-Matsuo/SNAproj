#include "EnemyBase.h"
#include "MeshComponent.h"
#include "System.h"

const EnemyBase::FlagType EnemyBase::mAliveFlagMask = 1 << 0;
const EnemyBase::FlagType EnemyBase::mFindPlayerFlagMask = 1 << 1;

EnemyBase::EnemyBase(char lifeMax) :
	mLifeMax(lifeMax),
	mLife(lifeMax),
	mFlags_Enemy(mAliveFlagMask)
{
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
}

EnemyBase::~EnemyBase()
{
}
