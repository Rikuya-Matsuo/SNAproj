#pragma once
#include "Actor.h"

class Mesh;

class AnimSpriteTest : public Actor
{
public:
	AnimSpriteTest();
	~AnimSpriteTest();

	void OnHit(const ColliderComponentBase * caller, ColliderAttribute colAtt) override;

private:
	Mesh * mMesh;
};

