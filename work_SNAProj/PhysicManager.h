#pragma once
#include "SDL/SDL.h"
#include <vector>
#include <list>
#include <unordered_map>
#include <thread>
#include <mutex>

class Actor;
class ColliderComponentBase;
typedef std::pair<ColliderComponentBase *, ColliderComponentBase *> ColliderPair;



// ファンクタ
class HashColliderPair final
{
public:
	HashColliderPair() {}
	
	size_t operator()(const ColliderPair& pair) const;
};



class PhysicManager final
{
public:
	~PhysicManager();

	static const float mGravityAcceleration;

	// 埋まっていると判断する割合
	// コライダーの辺がこのレート分埋まっていればOnBuryDeeply()を呼ぶ
	static const float mBuryRate;

	static PhysicManager & GetInstance()
	{
		static PhysicManager instance;
		return instance;
	}

	void ResisterCollider(const ColliderComponentBase * in_colCmp);

	void DeresisterCollider(const ColliderComponentBase * in_colCmp);

	void GravityAffect(Actor * actor) const;

	void CheckHit();

	void ResisterCheckableAttributeCombination(Uint8 att1, Uint8 att2);

	void ResisterCheckableAttributeCombination(std::pair<Uint8, Uint8>& pair);

	void RequestSortCollider(Uint8 attribute) { mSortAttributeList.emplace_back(attribute); }

	void ClearHitState();

	void ResisterHitIgnoreAttribute(const Actor * actor, Uint8 att);

	void DeresisterHitIgnoreAttribute(const Actor * actor, Uint8 att);

	void ClearHitIgnoreAttribute(const Actor * actor) { mHitIgnoreLists[actor].clear(); }

	friend class HashColliderPair;

private:
	enum HitState
	{
		HitState_NoTouch = 0,
		HitState_Hit,
		HitState_Touching,
		HitState_Invalid
	};

	PhysicManager();

	typedef std::lock_guard<std::mutex> MutexLocker;

	std::unordered_map<Uint8, std::vector<ColliderComponentBase *>> mColliders;

	std::unordered_map<ColliderPair, char, HashColliderPair> mHitColliderPairState;

	std::unordered_map<ColliderComponentBase *, unsigned short> mColliderID;

	std::list<std::pair<Uint8, Uint8>> mCheckableAttributeCombination;

	// Detectorアトリビュートの検出対象になるアトリビュート
	std::list<Uint8> mDetectSubjectList;

	std::list<Uint8> mSortAttributeList;

	// アクターが特定のアトリビュートと接触しなくなるよう一時的に設定できる機能のための変数群
	// どのアクターがどのアトリビュートと接触を一時無視するのか記録するための型
	typedef std::unordered_map<const Actor*, std::list<Uint8>> HitIgnoreList;

	HitIgnoreList mHitIgnoreLists;

	// NoTouch状態の接触情報を削除し続けるスレッド
	std::thread mRefreshThread;

	std::mutex mColliderPairStateMutex;

	bool mContinueRefleshFlag;

	bool mResetRefreshLoopFlag;

	// 当たり判定ループを行った回数のカウンター
	// 255を超えるときに0に戻す
	uint8_t mLoopCount;

	// RefreshHitState()内のループ終了時、mLoopCountの値を代入する
	// mLoopCountと比較して、値が異なる場合のみ、接触情報の整理を行う
	uint8_t mRefleshLoopCount;

	void HitPush(ColliderComponentBase * movalCol, const ColliderComponentBase * fixedCol);

	bool CheckPrevHit(const ColliderPair& pair);

	void CheckLoop(const std::pair<Uint8, Uint8>& attCombi);

	void HitProcess(ColliderPair& pair);

	void ApartProcess(ColliderPair& pair);

	void SetAttCombiSmallerFirst(std::pair<Uint8, Uint8>& pair);

	void ResetHitState(const ColliderComponentBase * col);

	// コライダーcolの接触記録をアトリビュートattに関するものだけNoTouchに設定する
	void ResetHitState(const ColliderComponentBase * col, Uint8 att);

	// 接触情報走査のループ回数が余計に増えるのを防ぐため、NoTouch状態の接触情報を全削除する関数
	void RefreshHitState();
	static void RefreshHitStateForThread()
	{
		PhysicManager::GetInstance().RefreshHitState();
	}

	void SortColliders();
};
