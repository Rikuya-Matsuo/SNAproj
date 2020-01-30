#pragma once
#include "Actor.h"

class Mesh;

class SpriteActor : public Actor
{
public:
	SpriteActor(const char * texPath);
	~SpriteActor();

	void UpdateActor1() override;

protected:
	Mesh * mMesh;
};
