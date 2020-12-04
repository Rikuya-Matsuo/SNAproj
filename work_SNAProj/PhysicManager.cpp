#include "PhysicManager.h"
#include "Actor.h"
#include "ColliderComponentBase.h"
#include "BoxColliderComponent.h"
#include "Collision.h"
#include "System.h"
#include "Common.h"
#include <cstdlib>
#include <algorithm>

const float PhysicManager::mGravityAcceleration = 1.0f;
const float PhysicManager::mBuryRate = 2.0f / 3.0f;

#ifdef DEBUG_SNA
//#define COLLISION_CHECK
static size_t checkCounter = 0;
#endif

void PhysicManager::CheckHit()
{
	// ミューテックスをロック
	MutexLocker lock(mColliderPairStateMutex);

	// 何かのアトリビュートでソートが要請されているならソートする
	if (!mSortAttributeList.empty())
	{
		SortColliders();
	}

	// Detectorを先に判定
	for (auto detectSub : mDetectSubjectList)
	{
		std::pair<Uint8, Uint8> attCombi =
			std::make_pair(ColliderAttribute::ColAtt_Detector, detectSub);

		CheckLoop(attCombi);
	}

	// 判定が有効なアトリビュートの組み合わせだけで判定
	for (auto attCombi : mCheckableAttributeCombination)
	{
		CheckLoop(attCombi);
	}

#ifdef COLLISION_CHECK
	SDL_Log("The number of checked collider is %d\n", checkCounter);
	checkCounter = 0;
#endif // DEBUG_SNA
}

void PhysicManager::CheckLoop(const std::pair<Uint8, Uint8>& attCombi)
{
	// いずれかのコライダーの数が0ならば判定を行わない
	if (mColliders[attCombi.first].empty() || mColliders[attCombi.second].empty())
	{
		return;
	}

	for (auto collider1 : mColliders[attCombi.first])
	{
		// コライダーがアクティブでないなら判定を行わない
		if (!collider1->GetActiveFlag())
		{
			// このコライダーを含む判定の組み合わせを当たっていない判定にする
			ResetHitState(collider1);

			continue;
		}

		// 形のデータを受け取る
		const AABB * IBox = collider1->GetWorldBox();
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
				ResetHitState(collider1);

				break;
			}

			// アクティブでないなら判定をスキップ
			if (!collider2->GetActiveFlag())
			{
				ResetHitState(collider2);

				continue;
			}

			// 球フラグ初期化
			sphereFlag &= ~2;

			// 形のデータを受け取る
			const AABB * JBox = collider2->GetWorldBox();
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

#ifdef COLLISION_CHECK
			// デバッグのため、チェックを記録
			checkCounter++;
#endif //COLLISION_CHECK

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
					bool ret =
						col->GetOwner()->GetMovalFlag() &&
						col->GetMoveOnHitFlag() &&
						(col->GetColliderAttribute() != ColliderAttribute::ColAtt_Detector);
					return ret;
				};
				bool movalFlag1 = checkMoval(collider1) && collider2->GetPushOnHitFlag();
				bool movalFlag2 = checkMoval(collider2) && collider1->GetPushOnHitFlag();
				bool sameOwner = collider1->GetOwner() == collider2->GetOwner();
				if (!sameOwner && movalFlag1)
				{
					HitPush(collider1, collider2);
				}
				if (!sameOwner && movalFlag2)
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
	auto itr = mHitColliderPairState.begin();
	for (; itr != mHitColliderPairState.end(); ++itr)
	{
		bool includedCollider =
			(itr->first.first == collider || itr->first.second == collider);

		if (includedCollider)
		{
			// 削除は別スレッドに任せる
			itr->second = HitState::HitState_NoTouch;
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
	// 移動ベクトル取得
	Vector3D vec = actor->GetMoveVector();

	// 重力分高さを下げる
	vec.z -= mGravityAcceleration * actor->GetFallSpeedRate() * System::GetInstance().GetDeltaTime();
	// アクターごとに決められた最大落下スピードを超えているなら調節
	const float limiter = actor->GetFallSpeedMax();
	if (vec.z < -fabsf(limiter))
	{
		vec.z = -limiter;
	}

	// 移動ベクトルセット
	actor->SetMoveVector(vec);
}

void PhysicManager::ResisterCheckableAttributeCombination(Uint8 att1, Uint8 att2)
{
	// Uint8型二つのペアを定義
	std::pair<Uint8, Uint8> pair = std::make_pair(att1, att2);

	// 組み合わせを登録
	ResisterCheckableAttributeCombination(pair);
}

void PhysicManager::ResisterCheckableAttributeCombination(std::pair<Uint8, Uint8>& pair)
{
	// どちらかがDetectorなら特別処理
	const Uint8 detectorAtt = ColliderAttribute::ColAtt_Detector;
	if (pair.first == detectorAtt || pair.second == detectorAtt)
	{
		// 両方か片方だけか
		(pair.first == pair.second) ?
			mCheckableAttributeCombination.emplace_back(pair) : 
			mDetectSubjectList.emplace_back((pair.first == detectorAtt) ? pair.second : pair.first);

		return;
	}

	// firstが値として小さいとルール付ける
	SetAttCombiSmallerFirst(pair);

	// 登録
	mCheckableAttributeCombination.emplace_back(pair);
}

void PhysicManager::ClearHitState()
{
	// ミューテックスをロック
	MutexLocker lock(mColliderPairStateMutex);

	// 全ての接触情報をクリア
	mHitColliderPairState.clear();

	// マルチスレッドで実行している接触情報整理ループをやり直すよう指示
	mResetRefreshLoopFlag = true;
}

// 奥行きを考えない押し戻し処理を使う
void PhysicManager::HitPush(ColliderComponentBase * movalCol, const ColliderComponentBase * fixedCol)
{
	// 両方ボックス
	if (movalCol->GetColliderShape() == ColliderShape::ColShape_Box &&
		fixedCol->GetColliderShape() == ColliderShape::ColShape_Box)
	{
		// ボックス取得
		const AABB * movalBox = movalCol->GetWorldBox();
		const AABB * fixedBox = fixedCol->GetWorldBox();

		// 各方向の接触面の大きさを計算
		auto checkContact = [](float aMin, float aMax, float bMin, float bMax)
		{
			bool aMinIsLargeThanBMin = aMin > bMin;
			bool aMaxIsLargeThanBMax = aMax > bMax;

			// 状況に応じて返却値を計算
			float contact;
			if (aMinIsLargeThanBMin && aMaxIsLargeThanBMax)
			{
				contact = bMax - aMin;
			}
			else if (aMinIsLargeThanBMin && !aMaxIsLargeThanBMax)
			{
				contact = aMax - aMin;
			}
			else if (!aMinIsLargeThanBMin && aMaxIsLargeThanBMax)
			{
				contact = bMax - bMin;
			}
			else
			{
				// 両方false
				contact = aMax - bMin;
			}

			return contact;
		};
		float contactX = checkContact(movalBox->mMin.x, movalBox->mMax.x, fixedBox->mMin.x, fixedBox->mMax.x);
		float contactZ = checkContact(movalBox->mMin.z, movalBox->mMax.z, fixedBox->mMin.z, fixedBox->mMax.z);

		// 深く埋まったかどうかの判定
		Vector3D movalBoxSize = movalBox->mMax - movalBox->mMin;
		bool buryDeeplyFlag =
			contactX >= movalBoxSize.x * mBuryRate &&
			contactZ >= movalBoxSize.z * mBuryRate;
		if (buryDeeplyFlag)
		{
			movalCol->GetOwner()->OnBuryDeeply();
		}

		// 絶対値を比較し、小さい方の（符号付きの）そのままの値を返す
		// サイズ２の配列で使うこと！
		auto absCmp = [](float * val)
		{
			return (fabsf(val[0]) < fabsf(val[1])) ? val[0] : val[1];
		};

		// アクターエイリアス取得
		Actor * movalActor = movalCol->GetOwner();

		// 押し返しベクトル
		Vector3D pushVec = Vector3D::zero;

		// zの接触面が大きければそれは壁であり、xの接触面が大きければそれは地面である
		if (contactZ > contactX)
		{
			// 壁

			float xOverlapCandidate[2];
			
			xOverlapCandidate[0] = movalBox->mMax.x - fixedBox->mMin.x;
			xOverlapCandidate[1] = movalBox->mMin.x - fixedBox->mMax.x;

			float xOverlap = absCmp(xOverlapCandidate);

			// 押し返し
			pushVec.x = -xOverlap;
			movalActor->SetFixVector(pushVec);

			return;
		}
		else
		{
			// 地面

			float zOverlapCandidate[2];

			zOverlapCandidate[0] = movalBox->mMax.z - fixedBox->mMin.z;
			zOverlapCandidate[1] = movalBox->mMin.z - fixedBox->mMax.z;

			float zOverlap = absCmp(zOverlapCandidate);

			// 押し返し
			pushVec.z = -zOverlap;
			movalActor->SetFixVector(pushVec);

			return;
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
	const char state = itr->second;
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
	// 値が小さい方をfirstにする
	if (pair.first > pair.second)
	{
		Uint8 tmp = pair.first;
		pair.first = pair.second;
		pair.second = tmp;
	}
}

void PhysicManager::ResetHitState(const ColliderComponentBase * col)
{
	// そのコライダーが関わった接触情報を探す
	auto itr = mHitColliderPairState.begin();
	for (; itr != mHitColliderPairState.end(); ++itr)
	{
		// そのコライダーが関係なければcontinue
		ColliderPair pair = itr->first;
		if (col != pair.first && col != pair.second)
		{
			continue;
		}

		// 接触状態である場合、両方の接触解除関数を呼び、接触情報を削除
		if (itr->second != HitState::HitState_NoTouch)
		{
			ApartProcess(pair);

			itr->second = HitState::HitState_NoTouch;
		}
	}
}

void PhysicManager::RefreshHitState()
{
	while (mContinueRefleshFlag)
	{
		auto itr = mHitColliderPairState.begin();
		for (; itr != mHitColliderPairState.end(); ++itr)
		{
			// ミューテックスをロック
			MutexLocker lock(mColliderPairStateMutex);

			// ループのやり直しを指示される or フラグがループの終了を示したとき、このfor文を抜ける
			if (mResetRefreshLoopFlag || !mContinueRefleshFlag)
			{
				mResetRefreshLoopFlag = false;
				break;
			}

			// 接触していないなら削除
			if (itr->second == HitState::HitState_NoTouch)
			{
				mHitColliderPairState.erase(itr);
			}
		}
	}
}

void PhysicManager::SortColliders()
{
	// ソート要請があったものをソートしていく
	for (auto attribute : mSortAttributeList)
	{
		auto sortByCheckOrder = [](const ColliderComponentBase * col1, const ColliderComponentBase * col2)
		{
			return col1->GetCheckOrder() <= col2->GetCheckOrder();
		};

		std::sort(mColliders[attribute].begin(), mColliders[attribute].end(), sortByCheckOrder);
	}

	// ソート要請をクリア
	mSortAttributeList.clear();
}

PhysicManager::PhysicManager():
	mContinueRefleshFlag(true),
	mResetRefreshLoopFlag(false)
{
	// コライダーコンポーネントのマップのメモリを確保
	mColliders.reserve(ColliderAttribute::ColAtt_Invalid);
	for (int i = 0; i < ColliderAttribute::ColAtt_Invalid; ++i)
	{
		mColliders[i].reserve(128);
	}

	// 各コンテナのメモリを確保
	mColliderID.reserve(256);
	mHitColliderPairState.reserve(32);

	// 当たり判定を行うアトリビュートの組み合わせを登録
	ResisterCheckableAttributeCombination(ColAtt_Player, ColAtt_Block);
	ResisterCheckableAttributeCombination(ColAtt_Player, ColAtt_Enemy);
	ResisterCheckableAttributeCombination(ColAtt_Detector, ColAtt_Block);
	ResisterCheckableAttributeCombination(ColAtt_Detector, ColAtt_Enemy);
	ResisterCheckableAttributeCombination(ColAtt_Detector, ColAtt_Player);
	ResisterCheckableAttributeCombination(ColAtt_PlayerAttack, ColAtt_Enemy);
	ResisterCheckableAttributeCombination(ColAtt_EnemyAttack, ColAtt_Player);
	ResisterCheckableAttributeCombination(ColAtt_Fire, ColAtt_Enemy);
	ResisterCheckableAttributeCombination(ColAtt_Fire, ColAtt_Block);

	// マルチスレッド作成およびメンバ変数へ引き渡し
	std::thread th(RefreshHitStateForThread);
	mRefreshThread.swap(th);
}

PhysicManager::~PhysicManager()
{
	// コライダーコンポーネントのマップをクリア
	for (int i = 0; i < ColliderAttribute::ColAtt_Invalid; ++i)
	{
		mColliders[i].clear();
		mColliders[i].shrink_to_fit();
		std::vector<ColliderComponentBase *>().swap(mColliders[i]);
	}
	std::unordered_map<Uint8, std::vector<ColliderComponentBase *>>().swap(mColliders);

	// コライダーIDのマップをクリア
	std::unordered_map<ColliderComponentBase *, unsigned short>().swap(mColliderID);

	// マルチスレッドを終了させる
	mContinueRefleshFlag = false;
	mRefreshThread.join();

	// 各コンテナクリア
	std::unordered_map<ColliderPair, char, HashColliderPair>().swap(mHitColliderPairState);

	std::list<std::pair<Uint8, Uint8>>().swap(mCheckableAttributeCombination);

	std::list<Uint8>().swap(mDetectSubjectList);
}



size_t HashColliderPair::operator()(const ColliderPair & pair) const
{
	// 物理マネージャの独立参照
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
