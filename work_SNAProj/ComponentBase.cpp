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
	mOwner->ResisterComponent(this);
}

ComponentBase::~ComponentBase()
{
	mOwner->DeresisterComponent(this);
}

void ComponentBase::Sleep()
{
	mFlags |= mSleepFlagMask;

	BitFlagFunc::SetFlagByBool(GetActiveFlag(), mFlags, mActiveFlagBeforeSleepMask);

	SetActive(false);
}

void ComponentBase::Wake()
{
	// Sleep()状態でないなら関数を実行しない
	if (!(mFlags & mSleepFlagMask))
	{
		return;
	}

	mFlags &= ~mSleepFlagMask;

	bool activeBeforeSleep = (mFlags & mActiveFlagBeforeSleepMask);
	SetActive(activeBeforeSleep);
}
