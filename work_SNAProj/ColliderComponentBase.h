#pragma once
#include "ComponentBase.h"
#include "Collision.h"

// Unityで言うところのTag
enum ColliderAttribute
{
	ColAtt_Player = 0,
	ColAtt_Enemy,
	ColAtt_Candle,
	ColAtt_Wall,
	ColAtt_Floor,
	ColAtt_Ceiling,
	ColAtt_Detector,		// トリガー。接近検知装置。ヒット時の押し返しなし。
	ColAtt_Invalid
};

enum ColliderShape
{
	ColShape_Sphere = 0,
	ColShape_Box,
	ColShape_Invalid
};

class ColliderComponentBase : public ComponentBase
{
public:
	ColliderComponentBase(Actor * owner, ColliderAttribute colAtt, ColliderShape colShape);
	virtual ~ColliderComponentBase();

	ColliderAttribute GetColliderAttribute() const { return static_cast<ColliderAttribute>(mAttribute); }

	ColliderShape GetColliderShape() const { return static_cast<ColliderShape>(mShape); }

	virtual const AABB * GetBox() const { return nullptr; }

	virtual const Sphere * GetSphere() const { return nullptr; }

	void SetHitReaction(void(*func)(ColliderAttribute colAtt)) { mHitReaction = func; }

	void ReactionOnHit(ColliderAttribute colAtt) { if (mHitReaction != nullptr) mHitReaction(colAtt); }

protected:
	const Uint8 mAttribute;

	const Uint8 mShape;

	void(*mHitReaction)(ColliderAttribute colAtt);
};
