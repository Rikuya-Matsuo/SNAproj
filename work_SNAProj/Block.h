#pragma once
#include "Actor.h"

class MeshComponent;

class Block : public Actor
{
public:
	Block();
	~Block();

	static const float mModelSize;

private:
	MeshComponent * mMeshComponent;
};

