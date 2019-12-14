﻿#pragma once
#include <vector>
#include <list>
#include <unordered_map>

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

	static PhysicManager & GetInstance()
	{
		static PhysicManager instance;
		return instance;
	}

	void ResisterCollider(const ColliderComponentBase * in_colCmp);

	void DeresisterCollider(const ColliderComponentBase * in_colCmp);

	void GravityAffect(Actor * actor) const;

	void CheckHit();

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

	const float mGravityAcceleration;

	std::vector<ColliderComponentBase *> mColliders;

	std::unordered_map<ColliderPair, char, HashColliderPair> mHitColliderPairState;

	std::unordered_map<ColliderComponentBase *, unsigned short> mColliderID;

	//std::list<unsigned short> mAssignedIDList;

	void HitPush(ColliderComponentBase * movalCol, const ColliderComponentBase * fixedCol);

	bool CheckPrevHit(const ColliderPair& pair);

	void HitProcess(ColliderPair& pair);

	void ApartProcess(ColliderPair& pair);
};
