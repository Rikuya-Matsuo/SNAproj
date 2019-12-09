#pragma once
#include <vector>
#include <unordered_map>

class ColliderComponentBase;

typedef std::pair<ColliderComponentBase *, ColliderComponentBase *> ColliderPair;

// ファンクタ
class HashColliderPair final
{
public:
	HashColliderPair() {}
	
	HashColliderPair(const ColliderPair& pair) {}

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

	unsigned short mForAssignColliderID;

	PhysicManager();

	std::vector<ColliderComponentBase *> mColliders;

	std::unordered_map<ColliderPair, char, HashColliderPair> mHitColliderPairState;

	std::unordered_map<ColliderComponentBase *, unsigned short> mColliderID;

	bool CheckPrevHit(const ColliderPair& pair);

	void HitProcess(ColliderPair& pair);

	void ApartProcess(ColliderPair& pair);
};
