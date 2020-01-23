#pragma once
#include "Actor.h"

class MeshComponent;

class Block : public Actor
{
public:
	Block(const std::string & texturePath, bool isGroundBlock);
	~Block();

	static const float mModelSize;

private:
	MeshComponent * mMeshComponent;
};

