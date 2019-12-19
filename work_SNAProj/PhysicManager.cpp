﻿#include "PhysicManager.h"
#include "Actor.h"
#include "ColliderComponentBase.h"
#include "BoxColliderComponent.h"
#include "Collision.h"
#include "System.h"
#include "Common.h"
#include <cstdlib>
#include <algorithm>

#define HIT_CHECK_TYPE 1
void PhysicManager::CheckHit()
{
	// 判定が有効なアトリビュートの組み合わせだけで判定
	for (auto attCombi : mCheckableAttributeCombination)
	{
		CheckLoop(attCombi);
	}

	// バージョンによってつぶさに切り替えられるようにする（完成したら整理しないと……）
#if HIT_CHECK_TYPE == 0
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

		// 連続で当たらなかった回数（ただし、一度何かと接触してからカウント開始）
		// 何とも当たっていないときは-1
		char noTouchCounter = -1;
		const char breakCheck = 4;

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

				// 接触なしカウンター始動
				noTouchCounter = 0;

				// 押し戻し
				bool iMovalFlag = mColliders[i]->GetOwner()->GetMovalFlag();
				bool jMovalFlag = mColliders[j]->GetOwner()->GetMovalFlag();
				if (iMovalFlag)
				{
					HitPush(mColliders[i], mColliders[j]);
				}
				if (jMovalFlag)
				{
					HitPush(mColliders[j], mColliders[i]);
				}
			}

			// 接触していなかった時の処理
			else
			{
				// 前フレームで接触していた場合
				if (prevHit)
				{
					ApartProcess(pair);
				}

				// 連続で接触しなかった回数をカウント
				// 規定回数に達したらコライダーiの接触判定終了
				if (noTouchCounter >= 0 && ++noTouchCounter >= breakCheck)
				{
					break;
				}
			}
		}
	}
#endif
}

void PhysicManager::CheckLoop(const std::pair<Uint8, Uint8>& attCombi)
{
	for (auto collider1 : mColliders[attCombi.first])
	{
		// コライダーがアクティブでないなら判定を行わない
		if (!collider1->GetActiveFlag())
		{
			continue;
		}

		// 形のデータを受け取る
		const AABB * IBox = collider1->GetBox();
		const Sphere * ISphere = collider1->GetSphere();

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

		for (auto collider2 : mColliders[attCombi.second])
		{
			// コライダー1がアクティブでなくなっているなら判定をブレーク
			if (!collider1->GetActiveFlag())
			{
				break;
			}

			// アクティブでないなら判定をスキップ
			if (!collider2->GetActiveFlag())
			{
				continue;
			}

			// 球フラグ初期化
			sphereFlag &= ~2;

			// 形のデータを受け取る
			const AABB * JBox = collider2->GetBox();
			const Sphere * JSphere = collider2->GetSphere();

			// 形を判別
			if (JSphere != nullptr)
			{
				// 球だけ取得できたなら、それは間違いなく球である。
				// どちらも取得出来たらエラー。このコライダーの判定を飛ばす。
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
				// どちらも取得できなかった場合もエラーとし、コライダーの判定を飛ばす。
				if (JBox == nullptr)
				{
					continue;
				}
			}

			// ペアを作成(IDの値が小さいほうをfirstとする)
			ColliderPair pair =
				mColliderID[collider1] < mColliderID[collider2] ?
				std::make_pair(collider1, collider2) :
				std::make_pair(collider2, collider1);

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
				// 押し戻し
				auto checkMoval = [](ColliderComponentBase* col)
				{
					return col->GetOwner()->GetMovalFlag() && (col->GetColliderAttribute() != ColliderAttribute::ColAtt_Detector);
				};
				bool iMovalFlag = checkMoval(collider1);
				bool jMovalFlag = checkMoval(collider2);
				if (iMovalFlag)
				{
					HitPush(collider1, collider2);
				}
				if (jMovalFlag)
				{
					HitPush(collider2, collider1);
				}

				HitProcess(pair);
			}

			// 接触していなかった時の処理
			else
			{
				// 前フレームで接触していた場合
				if (prevHit)
				{
					ApartProcess(pair);
				}
			}
		}
	}
}

void PhysicManager::ResisterCollider(const ColliderComponentBase * in_colCmp)
{
	ColliderComponentBase * collider = const_cast<ColliderComponentBase *>(in_colCmp);
	Uint8 attribute = collider->GetColliderAttribute();

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
		for (auto col : mColliders[attribute])
		{
			// 割り当てられていた場合、ID生成からやり直し
			if (mColliderID[col] == id)
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

	// コライダー登録
	mColliders[attribute].emplace_back(collider);
}

void PhysicManager::DeresisterCollider(const ColliderComponentBase * in_colCmp)
{
	ColliderComponentBase * collider = const_cast<ColliderComponentBase *>(in_colCmp);
	Uint8 attribute = collider->GetColliderAttribute();

	// 対象を含むコライダーペアの削除
	for (auto pair_att_colVec : mColliders)
	{
		for (auto colOfVec : pair_att_colVec.second)
		{
			ColliderPair pair =
				(mColliderID[collider] < mColliderID[colOfVec]) ?
				std::make_pair(collider, colOfVec) :
				std::make_pair(colOfVec, collider);

			if (mHitColliderPairState.count(pair))
			{
				mHitColliderPairState.erase(pair);
			}
		}
	}

	// IDを削除
	mColliderID.erase(collider);

	// コライダー配列から削除
	auto target = std::find(mColliders[attribute].begin(), mColliders[attribute].end(), collider);
	if (target != mColliders[attribute].end())
	{
		mColliders[attribute].erase(target);
	}

}

void PhysicManager::GravityAffect(Actor * actor) const
{
	Vector3D vec = actor->GetMoveVector();

	vec.z -= mGravityAcceleration * actor->GetFallSpeedRate() * System::GetInstance().GetDeltaTime();

	actor->SetMoveVector(vec);
}

void PhysicManager::ResisterCheckableAttributeCombination(Uint8 att1, Uint8 att2)
{
	std::pair<Uint8, Uint8> pair = std::make_pair(att1, att2);

	ResisterCheckableAttributeCombination(pair);
}

void PhysicManager::ResisterCheckableAttributeCombination(std::pair<Uint8, Uint8>& pair)
{
	// firstが値として小さいとルール付ける
	SetAttCombiSmallerFirst(pair);

	// 登録
	mCheckableAttributeCombination.emplace_back(pair);
}

// 奥行きを考えない押し戻し処理を使う
#define HITPUSH_AS_2D
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

		x[0] = movalBox->mMax.x - fixedBox->mMin.x;
		x[1] = movalBox->mMin.x - fixedBox->mMax.x;

#ifndef HITPUSH_AS_2D
		float y[2];
		y[0] = movalBox->mMax.y - fixedBox->mMin.y;
		y[1] = movalBox->mMin.y - fixedBox->mMax.y;
#endif

		float z[2];
		z[0] = movalBox->mMax.z - fixedBox->mMin.z;
		z[1] = movalBox->mMin.z - fixedBox->mMax.z;

		// floatの値 + ベクトルに格納していいかを示すフラグ
		struct NeoFloat
		{
			float value;
			bool flag;
		};

		// 2つのうち、めり込みの絶対値が小さいものを採用
		auto absCompete = [](float * f)
		{
			if (fabsf(f[0]) < fabsf(f[1]))
			{
				return f[0];
			}
			else
			{
				return f[1];
			}
		};
		NeoFloat smallerX; 
		smallerX.value = absCompete(x);

#ifndef HITPUSH_AS_2D
		NeoFloat smallerY;
		smallerY.value = absCompete(y);
#endif

		NeoFloat smallerZ;
		smallerZ.value = absCompete(z);

		// アクターのエイリアス取得
		Actor & movalActor = *movalCol->GetOwner();

		// 薄っぺらなメッシュもあるので、めり込みが0のものは不都合
		// かつ、まったく動いていない方向に修正されても困るので、移動ベクトルが0の成分も不都合
		// よって、そういったものは評価の対象外とする
		auto checkEvaluatable = [](NeoFloat& nf, float axisSpeed)
		{
			// 考え方：「評価対象となるのは、重なりも移動方向成分も非0であるもの」
			nf.flag = (nf.value != 0.0f) && (axisSpeed != 0.0f);
		};

		// ベクトルのエイリアス生成
		const Vector3D & vec = movalActor.GetMoveVector();

		checkEvaluatable(smallerX, vec.x);
#ifndef HITPUSH_AS_2D
		checkEvaluatable(smallerY, vec.y);
#endif
		checkEvaluatable(smallerZ, vec.z);

		// 一番移動量が小さくて済むベクトルを計算
		Vector3D pushVec = Vector3D::zero;
#ifndef HITPUSH_AS_2D
		auto isSmallestOf3 = [](const NeoFloat& value, const NeoFloat& comparison1, const NeoFloat& comparison2)
		{
			if (!value.flag)
			{
				return false;
			}

			// ビットフラグ
			Uint8 flag = 0;

			// 比較対象となりえて、値がvalueより大きい or 比較対象とならないなら真
			if ((comparison1.flag && value.value <= comparison1.value) || !comparison1.flag)
			{
				// 最下位ビットを立てる
				flag |= 1;
			}

			if ((comparison2.flag && value.value <= comparison2.value) || !comparison2.flag)
			{
				// 下から2ビット目を立てる
				flag |= 2;
			}

			// 下2ビットが両方立っていれば真を返す
			return flag == 3;
		};

		if (isSmallestOf3(smallerX, smallerY, smallerZ))
		{
			pushVec.x = -smallerX.value;
		}
		else if (isSmallestOf3(smallerY, smallerX, smallerZ))
		{
			pushVec.y = -smallerY.value;
		}
		else if (isSmallestOf3(smallerZ, smallerX, smallerY))
		{
			pushVec.z = -smallerZ.value;
		}
#else
		auto isSmallestOf2 = [](const NeoFloat& value, const NeoFloat& comparison)
		{
			if (!value.flag)
			{
				return false;
			}

			bool ret = false;

			if ((comparison.flag && value.value <= comparison.value) || !comparison.flag)
			{
				ret = true;
			}

			return ret;
		};

		if (isSmallestOf2(smallerX, smallerZ))
		{
			pushVec.x = -smallerX.value;
		}
		else if (isSmallestOf2(smallerZ, smallerX))
		{
			pushVec.z = -smallerZ.value;
		}
#endif // !HITPUSH_AS_2D

		// 次フレームで処理される位置修正のベクトルをアクターにセット
		movalActor.SetFixVector(pushVec);
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
	// コリジョンの組み合わせを検索
	auto itr = mHitColliderPairState.find(pair);

	// 検索がヒットしなかった or 接触状態が未接触だった場合
	if (itr == mHitColliderPairState.end() ||
		mHitColliderPairState[pair] == HitState::HitState_NoTouch)
	{
		// このフレームでの衝突
		mHitColliderPairState[pair] = HitState::HitState_Hit;

		// 衝突関数の呼び出し
		pair.first->OnHit(pair.second);
		pair.second->OnHit(pair.first);
	}

	// 前フレームで衝突した or 前フレームから接触していた場合
	else if (mHitColliderPairState[pair] == HitState::HitState_Hit ||
		mHitColliderPairState[pair] == HitState::HitState_Touching)
	{
		// 継続した接触
		mHitColliderPairState[pair] = HitState::HitState_Touching;

		// 接触関数の呼び出し
		pair.first->OnTouching(pair.second);
		pair.second->OnTouching(pair.first);
	}
}

void PhysicManager::ApartProcess(ColliderPair & pair)
{
	const ColliderAttribute att1st = pair.first->GetColliderAttribute();
	const ColliderAttribute att2nd = pair.second->GetColliderAttribute();

	// 状態記録を未接触に変更
	mHitColliderPairState[pair] = HitState::HitState_NoTouch;

	// 接触解除処理
	pair.first->OnApart(pair.second);
	pair.second->OnApart(pair.first);
}

void PhysicManager::SetAttCombiSmallerFirst(std::pair<Uint8, Uint8>& pair)
{
	if (pair.first > pair.second)
	{
		Uint8 tmp = pair.first;
		pair.first = pair.second;
		pair.second = tmp;
	}
}

PhysicManager::PhysicManager():
	mGravityAcceleration(9.8f)
{
	mColliders.reserve(ColliderAttribute::ColAtt_Invalid);
	for (int i = 0; i < ColliderAttribute::ColAtt_Invalid; ++i)
	{
		mColliders[i].reserve(128);
	}

	mColliderID.reserve(256);
	mHitColliderPairState.reserve(32);

	ResisterCheckableAttributeCombination(ColAtt_Player, ColAtt_Block);
	ResisterCheckableAttributeCombination(ColAtt_Player, ColAtt_Enemy);
}

PhysicManager::~PhysicManager()
{
	for (int i = 0; i < ColliderAttribute::ColAtt_Invalid; ++i)
	{
		std::vector<ColliderComponentBase *>().swap(mColliders[i]);
	}
	std::unordered_map<Uint8, std::vector<ColliderComponentBase *>>().swap(mColliders);

	std::unordered_map<ColliderComponentBase *, unsigned short>().swap(mColliderID);

	std::unordered_map<ColliderPair, char, HashColliderPair>().swap(mHitColliderPairState);

	std::list<std::pair<Uint8, Uint8>>().swap(mCheckableAttributeCombination);
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
