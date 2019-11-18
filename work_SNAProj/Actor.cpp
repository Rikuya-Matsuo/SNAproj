#include "Actor.h"
#include "System.h"
#include "ComponentBase.h"

Actor::Actor():
	mPosition(Vector3D::zero),
	mScale(1.0f)
{
	System::GetInstance().ResisterActor(this);
}

Actor::~Actor()
{
	System::GetInstance().DeresisterActor(this);
}

void Actor::Update()
{
	for (auto component : mComponentList)
	{
		component->Update();
	}
}

void Actor::ResisterComponent(const ComponentBase * in_cmp)
{
	const int priority = in_cmp->GetPriority();

	bool inserted = false;
	for (auto itr : mComponentList)
	{
		if (priority < itr->GetPriority())
		{
			auto insertPoint = std::find(mComponentList.begin(), mComponentList.end(), itr);
			mComponentList.insert(insertPoint, const_cast<ComponentBase*>(in_cmp));
			inserted = true;
			break;
		}
	}

	if (!inserted)
	{
		mComponentList.emplace_back(const_cast<ComponentBase*>(in_cmp));
	}
}

void Actor::DeresisterComponent(const ComponentBase * in_cmp)
{
	auto target = std::find(mComponentList.begin(), mComponentList.end(), const_cast<ComponentBase*>(in_cmp));

	if (target != mComponentList.end())
	{
		mComponentList.erase(target);
	}
}
