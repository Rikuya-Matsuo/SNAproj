#pragma once
#include "Actor.h"

class MeshComponent;

class Block : public Actor
{
public:
	Block(const std::string & texturePath, bool isGroundBlock);
	~Block();

	static const float mModelSize;

	void UpdateActor0() override;
	
	void UpdateActor1() override;

private:
	MeshComponent * mMeshComponent;

	bool mPrevInCameraFlag;
};

