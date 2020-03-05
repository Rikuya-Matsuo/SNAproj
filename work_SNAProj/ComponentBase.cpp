#include "ComponentBase.h"
#include "System.h"
#include "Actor.h"

const ComponentBase::FlagType ComponentBase::mActiveFlagMask = 1 << 0;

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
