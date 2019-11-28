#include "PhysicManager.h"
#include "ColliderComponentBase.h"

void PhysicManager::ResisterCollider(const ColliderComponentBase * in_colCmp)
{
	mColliders.emplace_back(const_cast<ColliderComponentBase *>(in_colCmp));
}

void PhysicManager::DeresisterCollider(const ColliderComponentBase * in_colCmp)
{
	auto target = std::find(mColliders.begin(), mColliders.end(), const_cast<ColliderComponentBase *>(in_colCmp));

	if (target != mColliders.end())
	{
		mColliders.erase(target);
	}
}

void PhysicManager::CheckHit()
{
	for (int i = 0; i < (int)mColliders.size() - 1; ++i)
	{
		// コライダーがアクティブでなければ中断
		if (!mColliders[i]->GetActiveFlag())
		{
			continue;
		}

		for (int j = i + 1; i < (int)mColliders.size(); ++j)
		{
			// 形のデータを受け取る

			// 当たっているかを判定
		}
	}
}

PhysicManager::PhysicManager()
{

}

PhysicManager::~PhysicManager()
{
	std::vector<ColliderComponentBase *>().swap(mColliders);
}
