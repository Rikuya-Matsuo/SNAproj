#pragma once
#include "Actor.h"

class Mesh;

class SpriteActor : public Actor
{
public:
	SpriteActor(const char * texPath, int drawOrder = 0);
	~SpriteActor();

	void UpdateActor1() override;

protected:
	Mesh * mMesh;
};
