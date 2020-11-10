#include "EnemyBase.h"
#include "MeshComponent.h"
#include "System.h"
#include "BlockHitChecker.h"

const EnemyBase::FlagType EnemyBase::mAliveFlagMask_EBase		= 1 << 0;
const EnemyBase::FlagType EnemyBase::mImmortalFlagMask_EBase	= 1 << 1;
const EnemyBase::FlagType EnemyBase::mLookRightFlagMask_EBase	= 1 << 2;
const EnemyBase::FlagType EnemyBase::mAttackFlagMask_EBase		= 1 << 3;
const EnemyBase::FlagType EnemyBase::mBeCapturedFlagMask_EBase	= 1 << 4;

const float EnemyBase::mDepth = 0.05f;

EnemyBase::EnemyBase(unsigned char lifeMax) :
	mLifeMax(lifeMax),
	mLife(lifeMax),
	mFlags_Enemy(mAliveFlagMask_EBase)
{
	// 更新優先度を設定
	SetPriority(500);

	// メッシュのロード
	mMesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Damage(unsigned char damageValue)
{
	// ダメージが正の値で、忍術によって捕まっている場合、一撃で死亡するダメージを受ける
	if (mFlags_Enemy & mBeCapturedFlagMask_EBase && damageValue > 0)
	{
		damageValue = mLifeMax;
	}
	mLife -= damageValue;
}

void EnemyBase::Capture()
{
	// 捕まったことを示すフラグを真にする
	BitFlagFunc::SetFlagByBool(true, mFlags_Enemy, mBeCapturedFlagMask_EBase);
}

void EnemyBase::LetGo()
{
	// 捕まったことを示すフラグを偽にする
	BitFlagFunc::SetFlagByBool(false, mFlags_Enemy, mBeCapturedFlagMask_EBase);
}

void EnemyBase::OnLifeRunOut()
{
	// 生存フラグを下す
	mFlags_Enemy &= ~mAliveFlagMask_EBase;
}

void EnemyBase::Flip()
{
	// 回転させる
	mRotationAngle = (mRotationAngle == 0.0f) ? static_cast<float>(M_PI) : 0.0f;
	OnFlip();
}

void EnemyBase::OnFlip()
{
}

void EnemyBase::UpdateActor0()
{
	// 死亡時の処理
	if (mLife <= 0 && !(mFlags_Enemy & mImmortalFlagMask_EBase))
	{
		OnLifeRunOut();
	}

	// 死亡した場合、更新を停止
	if (!(mFlags_Enemy & mAliveFlagMask_EBase))
	{
		mFlags |= mStopUpdateFlagMask_Base;
	}

	// エネミーのタイプ特有の処理
	UpdateEnemy0();
}

void EnemyBase::UpdateActor1()
{
	// エネミーのタイプ特有の処理
	UpdateEnemy1();

	// 奥行きの調整
	if (mPosition.y != mDepth)
	{
		mPosition.y = mDepth;
		mFlags |= mCalculateTransformFlagMask_Base;
	}
}
