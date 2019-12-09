#pragma once
#include <vector>
#include <unordered_map>

class ColliderComponentBase;

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

private:
	enum HitState
	{
		HitState_NoTouch = 0,
		HitState_Hit,
		HitState_Touching,
		HitState_Invalid
	};

	PhysicManager();

	std::vector<ColliderComponentBase *> mColliders;

	typedef std::pair<ColliderComponentBase *, ColliderComponentBase *> ColliderPair;

	std::unordered_map<ColliderPair, char> mHitColliderPairState;

	bool CheckPrevHit(const ColliderPair& pair);

	void HitProcess(ColliderPair& pair);

	void ApartProcess(ColliderPair& pair);
};
