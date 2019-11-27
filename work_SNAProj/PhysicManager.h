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

private:
	PhysicManager();

	std::vector<ColliderComponentBase *> mColliders;
};
