#pragma once
#include "Actor.h"

class Mesh;

class SpriteTest : public Actor
{
public:
	SpriteTest();
	~SpriteTest();

	void UpdateActor0() override;
	void UpdateActor1() override;

private:
	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	Mesh * mMesh;
};

