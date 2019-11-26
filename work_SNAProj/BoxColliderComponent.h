#pragma once
#include "ComponentBase.h"
#include "Vector.h"
#include "Collision.h"

// Unity‚ÅŒ¾‚¤‚Æ‚±‚ë‚ÌTag
enum ColliderAttribute
{
	ColAtt_Player = 0,
	ColAtt_Enemy,
	ColAtt_Candle,
	ColAtt_Invalid
};

class BoxColliderComponent final : public ComponentBase
{
public:
	BoxColliderComponent(Actor * owner, ColliderAttribute colAttribute = ColliderAttribute::ColAtt_Invalid);
	~BoxColliderComponent();
	
	void SetObjectBox(const AABB & objBox) { mObjectBox = objBox; }

	void Update() override;

	void CalculateWorldBox();

private:
	AABB mObjectBox;
	AABB mWorldBox;

	Uint8 mAttribute;
};
