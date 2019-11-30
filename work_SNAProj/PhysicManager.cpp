#include "PhysicManager.h"
#include "ColliderComponentBase.h"
#include "Collision.h"

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

		// �`�̃f�[�^���󂯎��
		const AABB * IBox = mColliders[i]->GetBox();
		const Sphere * ISphere = mColliders[i]->GetSphere();

		// �ǂ̌`���󂯎�ꂽ���𔻒肵�A�r�b�g�t���O�Ɋi�[
		// �ŉ��ʃr�b�g��i, ������2�Ԗڂ̃r�b�g��j�́A���擾�t���O
		Uint8 sphereFlag = 0;

		if (IBox == nullptr && ISphere != nullptr)
		{
			sphereFlag |= 1;
		}
		else
		{
			// ���̎��A�����擾�ł���or�ł��Ȃ������B
			// ����ăG���[�B���̕��̂̓����蔻����s��Ȃ��B
			continue;
		}

		for (int j = i + 1; i < (int)mColliders.size(); ++j)
		{
			// �R���C�_�[�̃A�N�e�B�u����
			if (!mColliders[j]->GetActiveFlag())
			{
				continue;
			}

			// �`�̃f�[�^���󂯎��
			const AABB * JBox = mColliders[j]->GetBox();
			const Sphere * JSphere = mColliders[j]->GetSphere();

			// �t���O������
			sphereFlag &= ~2;

			// �󂯎�����`�𔻒�
			if (JBox == nullptr && JSphere != nullptr)
			{
				sphereFlag |= 2;
			}
			
			// �������Ă��邩�𔻒�
			bool hit = false;
			switch (sphereFlag)
			{
			case 0:
				hit = Collision::CheckHit(*IBox, *JBox);
				break;
			case 1:
				hit = Collision::CheckHit(*ISphere, *JBox);
				break;
			case 2:
				hit = Collision::CheckHit(*JSphere, *IBox);
				break;
			case 3:
				hit = Collision::CheckHit(*ISphere, *JSphere);
				break;
			default:
				break;
			}

			// �q�b�g���̃��A�N�V����
			if (hit)
			{
				const ColliderAttribute iAtt = mColliders[i]->GetColliderAttribute();
				const ColliderAttribute jAtt = mColliders[j]->GetColliderAttribute();
				mColliders[i]->ReactionOnHit(jAtt);
				mColliders[j]->ReactionOnHit(iAtt);
			}
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
