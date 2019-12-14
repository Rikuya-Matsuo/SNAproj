#include "PhysicManager.h"
#include "Actor.h"
#include "ColliderComponentBase.h"
#include "BoxColliderComponent.h"
#include "Collision.h"
#include "System.h"
#include "Common.h"
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

void PhysicManager::GravityAffect(Actor * actor) const
{
	Vector3D vec = actor->GetMoveVector();

	vec.z -= mGravityAcceleration * actor->GetFallSpeedRate() * System::GetInstance().GetDeltaTime();

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

void PhysicManager::HitPush(ColliderComponentBase * movalCol, const ColliderComponentBase * fixedCol)
{
	// 両方ボックス
	if (movalCol->GetColliderShape() == ColliderShape::ColShape_Box &&
		fixedCol->GetColliderShape() == ColliderShape::ColShape_Box)
	{
		// ボックス取得
		const AABB * movalBox = movalCol->GetBox();
		const AABB * fixedBox = fixedCol->GetBox();

		// 各成分のめり込み量計算
		// 1成分につき2方向からのめり込みを計算する必要があるので、各2個の配列を生成している
		float x[2];
		float y[2];
		float z[2];

		x[0] = movalBox->mMax.x - fixedBox->mMin.x;
		x[1] = fixedBox->mMax.x - movalBox->mMin.x;

		y[0] = movalBox->mMax.y - fixedBox->mMin.y;
		y[1] = fixedBox->mMax.y - movalBox->mMin.y;

		z[0] = movalBox->mMax.z - fixedBox->mMin.z;
		z[1] = fixedBox->mMax.z - movalBox->mMin.z;

		// floatの値 + ベクトルに格納していいかを示すフラグ
		struct NeoFloat
		{
			float value;
			bool flag;
		};

		// 2つのうち、めり込みが小さいものを採用
		NeoFloat smallerX, smallerY, smallerZ;
		smallerX.value = Common::Smaller(fabsf(x[0]), fabsf(x[1]));
		smallerY.value = Common::Smaller(fabsf(y[0]), fabsf(y[1]));
		smallerZ.value = Common::Smaller(fabsf(z[0]), fabsf(z[1]));

		// 薄っぺらなメッシュもあるので、めり込みが0のものは不都合
		// よって、そういったものは評価の対象外とする
		auto checkEvaluatable = [](NeoFloat nf)
		{
			nf.flag = (nf.value == 0.0f) ? false : true;
		};
		checkEvaluatable(smallerX);
		checkEvaluatable(smallerY);
		checkEvaluatable(smallerZ);

		// 一番移動量が小さくて済むベクトルを計算
		Vector3D pushVec = Vector3D::zero;
		auto isSmallestOf3 = [](NeoFloat value, NeoFloat comparison1, NeoFloat comparison2)
		{
			if (!value.flag)
			{
				return false;
			}

			///////////////////////////
			// つぎはここから！
			///////////////////////////

		};

		if (isSmallestOf3(smallerX, smallerY, smallerZ))
		{

		}
	}

	// それ以外のケースは必要に応じて実装する
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
