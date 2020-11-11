#include "ComponentBase.h"
#include "System.h"
#include "Actor.h"

const ComponentBase::FlagType ComponentBase::mActiveFlagMask				= 1 << 0;
const ComponentBase::FlagType ComponentBase::mActiveFlagBeforeSleepMask		= 1 << 1;
const ComponentBase::FlagType ComponentBase::mSleepFlagMask					= 1 << 2;

ComponentBase::ComponentBase(Actor * owner, int priority, UpdateTiming updateTiming) :
	mOwner(owner),
	mPriority(priority),
	mFlags(mActiveFlagMask),
	mUpdateTiming(updateTiming)
{
	// アクターに自身を登録
	mOwner->ResisterComponent(this);
}

ComponentBase::~ComponentBase()
{
	// アクターから自身を登録解除
	mOwner->DeresisterComponent(this);
}

void ComponentBase::Sleep()
{
	// 一時停止フラグを真に
	mFlags |= mSleepFlagMask;

	// 一時停止前のアクティブ状態を記録
	BitFlagFunc::SetFlagByBool(GetActiveFlag(), mFlags, mActiveFlagBeforeSleepMask);

	// 非アクティブに
	SetActive(false);
}

void ComponentBase::Wake()
{
	// 一時停止状態でないなら関数を実行しない
	if (!(mFlags & mSleepFlagMask))
	{
		return;
	}

	// 一時停止フラグを偽に
	mFlags &= ~mSleepFlagMask;

	// 一時停止前のアクティブ状態に戻す
	bool activeBeforeSleep = (mFlags & mActiveFlagBeforeSleepMask);
	SetActive(activeBeforeSleep);
}
