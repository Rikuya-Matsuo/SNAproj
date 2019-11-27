#pragma once
#include "ColliderComponentBase.h"
#include "Vector.h"
#include "Collision.h"

class BoxColliderComponent final : public ColliderComponentBase
{
public:
	BoxColliderComponent(Actor * owner, ColliderAttribute colAttribute);
	~BoxColliderComponent();
	
	void SetObjectBox(const AABB & objBox) { mObjectBox = objBox; }

	void Update() override;

	void CalculateWorldBox();

private:
	AABB mObjectBox;
	AABB mWorldBox;
};
