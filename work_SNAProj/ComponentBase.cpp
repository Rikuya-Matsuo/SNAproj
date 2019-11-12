#include "ComponentBase.h"
#include "System.h"
#include "Actor.h"

ComponentBase::ComponentBase(Actor * owner, int priority) :
	mOwner(owner),
	mPriority(priority)
{
	mOwner->ResisterComponent(this);
}

ComponentBase::~ComponentBase()
{
	mOwner->DeresisterComponent(this);
}
