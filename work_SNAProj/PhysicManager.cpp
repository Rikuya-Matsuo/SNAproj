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
		// �R���C�_�[���A�N�e�B�u�łȂ���Β��f
		if (!mColliders[i]->GetActiveFlag())
		{
			continue;
		}

		for (int j = i + 1; i < (int)mColliders.size(); ++j)
		{
			// �`�̃f�[�^���󂯎��

			// �������Ă��邩�𔻒�
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
