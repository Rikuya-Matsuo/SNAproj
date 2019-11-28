#pragma once
#include <vector>

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
	PhysicManager();

	std::vector<ColliderComponentBase *> mColliders;
};
