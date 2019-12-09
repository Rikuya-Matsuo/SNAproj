#include "PhysicManager.h"
#include "ColliderComponentBase.h"
#include "BoxColliderComponent.h"
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

		// �`�𔻕�
		if (ISphere != nullptr)
		{
			// �������擾�ł����Ȃ�A����͊ԈႢ�Ȃ����ł���B
			// �ǂ�����擾�o������G���[�B���̃R���C�_�[�̔�����΂��B
			if (IBox == nullptr)
			{
				sphereFlag |= 1;
			}
			else
			{
				continue;
			}
		}
		else
		{
			// �ǂ�����擾�ł��Ȃ������ꍇ���G���[�Ƃ��A�R���C�_�[�̔�����΂��B
			if (IBox == nullptr)
			{
				continue;
			}
		}

		for (int j = i + 1; j < (int)mColliders.size(); ++j)
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
			if (JSphere != nullptr)
			{
				if (JBox == nullptr)
				{
					sphereFlag |= 2;
				}
				else
				{
					// �G���[�F�ǂ�����擾�ł���
					continue;
				}
			}
			else
			{
				if (JBox == nullptr)
				{
					// �G���[�F�ǂ�����擾�ł��Ȃ�����
					continue;
				}
			}

			// �y�A���쐬
			ColliderPair pair = std::make_pair(mColliders[i], mColliders[j]);

			// �O�̃t���[���ł̐ڐG��Ԃ𔻒�
			bool prevHit = CheckPrevHit(pair);

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
				HitProcess(pair);
			}

			// �ڐG���Ă��Ȃ��������̏���
			// �������A�O�t���[���ŐڐG���Ă����ꍇ�̂�
			else if (prevHit)
			{
				ApartProcess(pair);
			}
		}
	}
}

bool PhysicManager::CheckPrevHit(const ColliderPair& pair)
{
	// �y�A������
	auto itr = mHitColliderPairState.find(pair);

	// �����Ƀq�b�g���Ȃ�����
	if (itr == mHitColliderPairState.end())
	{
		return false;
	}

	// �ԋp�l
	bool ret = false;

	// �O�t���[���̐ڐG�𔻒�
	const char state = mHitColliderPairState[pair];
	if (state == HitState::HitState_Touching || state == HitState::HitState_Hit)
	{
		ret = true;
	}

	return ret;
}

void PhysicManager::HitProcess(ColliderPair& pair)
{
	const ColliderAttribute att1st = pair.first->GetColliderAttribute();
	const ColliderAttribute att2nd = pair.second->GetColliderAttribute();

	// �R���W�����̑g�ݍ��킹������
	auto itr = mHitColliderPairState.find(pair);

	// �������q�b�g���Ȃ����� or �ڐG��Ԃ����ڐG�������ꍇ
	if (itr == mHitColliderPairState.end() ||
		mHitColliderPairState[pair] == HitState::HitState_NoTouch)
	{
		// ���̃t���[���ł̏Փ�
		mHitColliderPairState[pair] = HitState::HitState_Hit;

		// �Փˊ֐��̌Ăяo��
		pair.first->OnHit(att2nd);
		pair.second->OnHit(att1st);
	}

	// �O�t���[���ŏՓ˂��� or �O�t���[������ڐG���Ă����ꍇ
	else if (mHitColliderPairState[pair] == HitState::HitState_Hit ||
		mHitColliderPairState[pair] == HitState::HitState_Touching)
	{
		// �p�������ڐG
		mHitColliderPairState[pair] = HitState::HitState_Touching;

		// �ڐG�֐��̌Ăяo��
		pair.first->OnTouching(att2nd);
		pair.second->OnTouching(att1st);
	}
}

void PhysicManager::ApartProcess(ColliderPair & pair)
{
	const ColliderAttribute att1st = pair.first->GetColliderAttribute();
	const ColliderAttribute att2nd = pair.second->GetColliderAttribute();

	// ��ԋL�^�𖢐ڐG�ɕύX
	mHitColliderPairState[pair] = HitState::HitState_NoTouch;

	// �ڐG��������
	pair.first->OnApart(att2nd);
	pair.second->OnApart(att1st);
}

PhysicManager::PhysicManager()
{
	mHitColliderPairState.reserve(32);
}

PhysicManager::~PhysicManager()
{
	std::vector<ColliderComponentBase *>().swap(mColliders);
}
