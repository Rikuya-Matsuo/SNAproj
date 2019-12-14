#include "PhysicManager.h"
#include "Actor.h"
#include "ColliderComponentBase.h"
#include "BoxColliderComponent.h"
#include "Collision.h"
#include <cstdlib>

void PhysicManager::ResisterCollider(const ColliderComponentBase * in_colCmp)
{
	ColliderComponentBase * collider = const_cast<ColliderComponentBase *>(in_colCmp);

	// ループを減らすため、ランダムを使ってIDを設定する方法を使う。
	bool loop = true;
	while (loop)
	{
		// IDをランダム生成
		// rand()は15ビットの範囲で乱数を出すらしいので、
		// 頭の1ビットをランダムで決定することで、unsigned short型の最大値までのIDを発行できるようにする。
		unsigned short usRand = rand() | ((rand() & 1) ? 0x8000 : 0);
		unsigned short id = usRand % (UINT16_MAX + 1);		// 念のため余剰演算法を使っておく

		// そのIDが割り当てられているかを走査
		bool alreadyAssigned = false;
		for (unsigned int i = 0; i < mColliders.size(); ++i)
		{
			// 割り当てられていた場合、ID生成からやり直し
			if (mColliderID[mColliders[i]] == id)
			{
				alreadyAssigned = true;
				break;
			}
		}

		if (alreadyAssigned)
		{
			continue;
		}
		else
		{
			mColliderID[collider] = id;
			loop = false;
		}
	}

	// ↓もう一つのID設定方法。ループ使うのがちょっと気に入らないのでもう少し検討する。
	/*
	unsigned short i = 0;
	bool loop = true;
	while (loop)
	{
		// その数字が割り当てられていないとき
		auto itr = std::find(mAssignedIDList.begin(), mAssignedIDList.end(), i);
		if (itr == mAssignedIDList.end())
		{
			mColliderID[collider] = i;
			mAssignedIDList.emplace_back(i++);
			loop = false;
		}
	}
	*/

	// コライダー登録
	mColliders.emplace_back(collider);
}

void PhysicManager::DeresisterCollider(const ColliderComponentBase * in_colCmp)
{
	ColliderComponentBase * collider = const_cast<ColliderComponentBase *>(in_colCmp);

	// 対象を含むコライダーペアの削除
	for (int i = 0; mColliders[i] != collider; ++i)
	{
		ColliderPair pair = std::make_pair(mColliders[i], collider);

		if (mHitColliderPairState.count(pair))
		{
			mHitColliderPairState.erase(pair);
		}
	}

	for (int i = mColliders.size() - 1; mColliders[i] != collider; --i)
	{
		ColliderPair pair = std::make_pair(collider, mColliders[i]);

		if (mHitColliderPairState.count(pair))
		{
			mHitColliderPairState.erase(pair);
		}
	}

	// IDを削除
	mColliderID.erase(collider);

	// コライダー配列から削除
	auto target = std::find(mColliders.begin(), mColliders.end(), collider);
	if (target != mColliders.end())
	{
		mColliders.erase(target);
	}

}

void PhysicManager::GravityAffect(Actor * actor)
{
	Vector3D vec = actor->GetMoveVector();

	vec.z -= mGravityAcceleration * actor->GetFallSpeedRate();

	actor->SetMoveVector(vec);
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
					// エラー：どちらも取得できた
					continue;
				}
			}
			else
			{
				if (JBox == nullptr)
				{
					// エラー：どちらも取得できなかった
					continue;
				}
			}

			// ペアを作成(IDの値が小さいほうをfirstとする)
			ColliderPair pair = 
				mColliderID[mColliders[i]] < mColliderID[mColliders[j]] ?
				std::make_pair(mColliders[i], mColliders[j]) :
				std::make_pair(mColliders[j], mColliders[i]);

			// 前のフレームでの接触状態を判定
			bool prevHit = CheckPrevHit(pair);

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
				HitProcess(pair);
			}

			// 接触していなかった時の処理
			// ただし、前フレームで接触していた場合のみ
			else if (prevHit)
			{
				ApartProcess(pair);
			}
		}
	}
}

bool PhysicManager::CheckPrevHit(const ColliderPair& pair)
{
	// ペアを検索
	auto itr = mHitColliderPairState.find(pair);

	// 検索にヒットしなかった
	if (itr == mHitColliderPairState.end())
	{
		return false;
	}

	// 返却値
	bool ret = false;

	// 前フレームの接触を判定
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

	// コリジョンの組み合わせを検索
	auto itr = mHitColliderPairState.find(pair);

	// 検索がヒットしなかった or 接触状態が未接触だった場合
	if (itr == mHitColliderPairState.end() ||
		mHitColliderPairState[pair] == HitState::HitState_NoTouch)
	{
		// このフレームでの衝突
		mHitColliderPairState[pair] = HitState::HitState_Hit;

		// 衝突関数の呼び出し
		pair.first->OnHit(att2nd);
		pair.second->OnHit(att1st);
	}

	// 前フレームで衝突した or 前フレームから接触していた場合
	else if (mHitColliderPairState[pair] == HitState::HitState_Hit ||
		mHitColliderPairState[pair] == HitState::HitState_Touching)
	{
		// 継続した接触
		mHitColliderPairState[pair] = HitState::HitState_Touching;

		// 接触関数の呼び出し
		pair.first->OnTouching(att2nd);
		pair.second->OnTouching(att1st);
	}
}

void PhysicManager::ApartProcess(ColliderPair & pair)
{
	const ColliderAttribute att1st = pair.first->GetColliderAttribute();
	const ColliderAttribute att2nd = pair.second->GetColliderAttribute();

	// 状態記録を未接触に変更
	mHitColliderPairState[pair] = HitState::HitState_NoTouch;

	// 接触解除処理
	pair.first->OnApart(att2nd);
	pair.second->OnApart(att1st);
}

PhysicManager::PhysicManager():
	mGravityAcceleration(9.8f / 100)
{
	mColliders.reserve(128);
	mColliderID.reserve(128);
	mHitColliderPairState.reserve(32);
}

PhysicManager::~PhysicManager()
{
	std::vector<ColliderComponentBase *>().swap(mColliders);
}



size_t HashColliderPair::operator()(const ColliderPair & pair) const
{
	PhysicManager& physic = PhysicManager::GetInstance();

	// ハッシュ値として、上２バイトと下２バイトにそれぞれのID値を入れたunsigned int型の値を生成する
	union Hash
	{
		size_t uInt;
		unsigned short uShort[2];
	};

	Hash ret;
	ret.uShort[0] = physic.mColliderID[pair.first];
	ret.uShort[1] = physic.mColliderID[pair.second];

	return ret.uInt;
}
