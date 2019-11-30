#include "Actor.h"
#include "System.h"
#include "Common.h"
#include "ComponentBase.h"
#include <algorithm>

const Actor::FlagType Actor::mRequestComponentSortMask = 1 << 0;
const Actor::FlagType Actor::mStopDrawFlagMask = 1 << 1;
const Actor::FlagType Actor::mBeyondSceneFlagMask = 1 << 2;

Actor::Actor():
	mPosition(Vector3D::zero),
	mScale(1.0f),
	mFlags(0)
{
	System::GetInstance().ResisterActor(this);
}

Actor::~Actor()
{
	Common::DeleteContainerOfPointer(mComponentList);
	std::list<ComponentBase *>().swap(mComponentList);

	System::GetInstance().DeresisterActor(this);
}

void Actor::Update()
{
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

void Actor::UpdateComponents()
{
	// コンポーネントのソートがリクエストされていた場合はソートを行う
	if (mFlags & mRequestComponentSortMask)
	{
		SortComponents();

		// フラグを下す
		mFlags &= ~mRequestComponentSortMask;
	}

	for (auto component : mComponentList)
	{
		component->Update();
	}
}

void Actor::SortComponents()
{
	// 見よ！これがラムダ式を使ったソートである！
	mComponentList.sort([](ComponentBase * lhs, ComponentBase * rhs) { return lhs->GetPriority() < rhs->GetPriority(); });
}

void Actor::UpdateActor()
{
}

void Actor::CalculateWorldTransform()
{
	mWorldTransform = Matrix4::CreateScale(mScale);
	mWorldTransform *= Matrix4::CreateTranslation(mPosition);
}

void Actor::OnHit(ColliderAttribute colAtt)
{

}
