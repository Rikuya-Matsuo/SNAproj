#include "ComponentManager.h"
#include "ComponentBase.h"

ComponentManager::ComponentManager()
{
}

void ComponentManager::UpdateComponents()
{
	for (auto cmp : mComponentList)
	{
		cmp->Update();
	}
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::ResisterComponent(ComponentBase * in_cmp)
{
	auto itr = mComponentList.begin();
	for (; itr != mComponentList.end(); ++itr)
	{
		if (in_cmp->GetPriority() < (*itr)->GetPriority())
		{
			mComponentList.insert(itr, in_cmp);
			break;
		}
	}

	if (itr == mComponentList.end())
	{
		mComponentList.emplace_back(in_cmp);
	}
}

void ComponentManager::DeresisterComponent(ComponentBase * in_cmp)
{
	auto itr = std::find(mComponentList.begin(), mComponentList.end(), in_cmp);

	if (itr != mComponentList.end())
	{
		mComponentList.erase(itr);
	}
}
