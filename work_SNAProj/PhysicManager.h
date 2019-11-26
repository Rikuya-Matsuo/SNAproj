#pragma once
#include <vector>

class BoxColliderComponent;

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

	std::vector<BoxColliderComponent *> mColliders;
};
