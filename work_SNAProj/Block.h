#pragma once
#include "Actor.h"

class MeshComponent;

class Block : public Actor
{
public:
	Block();
	~Block();

	

private:
	MeshComponent * mMeshComponent;
};

