#include "ComponentBase.h"
#include "System.h"
#include "Actor.h"
#include "ComponentManager.h"

//std::list<ComponentBase *> mComponentList;

ComponentBase::ComponentBase(Actor * owner, int priority) :
	mOwner(owner),
	mPriority(priority)
{
	mOwner->ResisterComponent(this);

	ComponentManager::GetInstance().ResisterComponent(this);

	/*
	bool inserted = false;
	for (auto cmp : mComponentList)
	{
		if (this->mPriority < cmp->mPriority)
		{
			auto itr = std::find(mComponentList.begin(), mComponentList.end(), cmp);
			mComponentList.insert(itr, this);
			inserted = true;
			break;
		}
	}

	if (!inserted)
	{
		mComponentList.emplace_back(this);
	}
	*/
}

ComponentBase::~ComponentBase()
{
	mOwner->DeresisterComponent(this);
	
	ComponentManager::GetInstance().DeresisterComponent(this);

	/*
	auto itr = std::find(mComponentList.begin(), mComponentList.end(), this);
	if (itr != mComponentList.end())
	{
		mComponentList.erase(itr);
	}
	*/
}

/*
void ComponentBase::UpdateComponents()
{
	for (auto cmp : mComponentList)
	{
		cmp->Update();
	}
}
*/
