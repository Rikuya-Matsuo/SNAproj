#pragma once
#include "ComponentBase.h"
#include "Vector.h"
#include "Collision.h"

// Unity‚ÅŒ¾‚¤‚Æ‚±‚ë‚ÌTag
enum ColliderAttribute
{
	Player,
	Enemy,
	Candle,
	Invalid
};

class BoxColliderComponent final : public ComponentBase
{
public:
	BoxColliderComponent(Actor * owner, ColliderAttribute colAttribute = ColliderAttribute::Invalid);
	~BoxColliderComponent();
	
	void SetObjectBox(const AABB & objBox) { mObjectBox = objBox; }

private:
	AABB mObjectBox;
	AABB mWorldBox;

	Uint8 mAttribute;
};
