#include "ComponentBase.h"
#include "System.h"
#include "Actor.h"
#include "ComponentManager.h"

ComponentBase::ComponentBase(Actor * owner, int priority) :
	mOwner(owner),
	mPriority(priority)
{
	mOwner->ResisterComponent(this);

	ComponentManager::GetInstance().ResisterComponent(this);
}

ComponentBase::~ComponentBase()
{
	mOwner->DeresisterComponent(this);
	
	ComponentManager::GetInstance().DeresisterComponent(this);
}
