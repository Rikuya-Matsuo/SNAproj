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
		// コライダーがアクティブでなければ中断
		if (!mColliders[i]->GetActiveFlag())
		{
			continue;
		}

		// 形のデータを受け取る
		const AABB * IBox = mColliders[i]->GetBox();
		const Sphere * ISphere = mColliders[i]->GetSphere();

		// どの形が受け取れたかを判定し、ビットフラグに格納
		// 最下位ビットがi, 下から2番目のビットがjの、球取得フラグ
		Uint8 sphereFlag = 0;

		// 形を判別
		if (ISphere != nullptr)
		{
			// 球だけ取得できたなら、それは間違いなく球である。
			// どちらも取得出来たらエラー。このコライダーの判定を飛ばす。
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
			// どちらも取得できなかった場合もエラーとし、コライダーの判定を飛ばす。
			if (IBox == nullptr)
			{
				continue;
			}
		}

		for (int j = i + 1; j < (int)mColliders.size(); ++j)
		{
			// コライダーのアクティブ判定
			if (!mColliders[j]->GetActiveFlag())
			{
				continue;
			}

			// 形のデータを受け取る
			const AABB * JBox = mColliders[j]->GetBox();
			const Sphere * JSphere = mColliders[j]->GetSphere();

			// フラグ初期化
			sphereFlag &= ~2;

			// 受け取った形を判定
			if (JSphere != nullptr)
			{
				if (JBox == nullptr)
				{
					sphereFlag |= 2;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (JBox == nullptr)
				{
					continue;
				}
			}
			
			// 当たっているかを判定
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

			// ヒット時のリアクション
			if (hit)
			{
				const ColliderAttribute iAtt = mColliders[i]->GetColliderAttribute();
				const ColliderAttribute jAtt = mColliders[j]->GetColliderAttribute();
				mColliders[i]->OnHit(jAtt);
				mColliders[j]->OnHit(iAtt);
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
