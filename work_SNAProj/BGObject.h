#pragma once
#include "Actor.h"

class MeshComponent;

class BGObject : public Actor
{
public:
	BGObject(const std::string & meshFileName);
	~BGObject();

private:
	MeshComponent * mMeshComponent;
};
