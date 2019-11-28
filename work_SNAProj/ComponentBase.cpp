#include "ComponentBase.h"
#include "System.h"
#include "Actor.h"
#include "ComponentManager.h"

const ComponentBase::FlagType ComponentBase::mNotActiveFlagMask = 1 << 0;

ComponentBase::ComponentBase(Actor * owner, int priority) :
	mOwner(owner),
	mPriority(priority),
	mFlags(0)
{
	mOwner->ResisterComponent(this);

	ComponentManager::GetInstance().ResisterComponent(this);
}

ComponentBase::~ComponentBase()
{
	mOwner->DeresisterComponent(this);
	
	ComponentManager::GetInstance().DeresisterComponent(this);
}
