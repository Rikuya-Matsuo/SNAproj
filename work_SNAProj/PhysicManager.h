#pragma once
#include "SDL/SDL.h"
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

	void ResisterCheckableAttributeCombination(Uint8 att1, Uint8 att2);

	void ResisterCheckableAttributeCombination(std::pair<Uint8, Uint8>& pair);

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

	std::unordered_map<Uint8, std::vector<ColliderComponentBase *>> mColliders;

	std::unordered_map<ColliderPair, char, HashColliderPair> mHitColliderPairState;

	std::unordered_map<ColliderComponentBase *, unsigned short> mColliderID;

	std::list<std::pair<Uint8, Uint8>> mCheckableAttributeCombination;

	//std::list<unsigned short> mAssignedIDList;

	void HitPush(ColliderComponentBase * movalCol, const ColliderComponentBase * fixedCol);

	bool CheckPrevHit(const ColliderPair& pair);

	void CheckLoop(const std::pair<Uint8, Uint8>& attCombi);

	void HitProcess(ColliderPair& pair);

	void ApartProcess(ColliderPair& pair);

	void SetAttCombiSmallerFirst(std::pair<Uint8, Uint8>& pair);
};
