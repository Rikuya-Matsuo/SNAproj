#pragma once
#include "ComponentBase.h"

// Unityで言うところのTag
enum ColliderAttribute
{
	ColAtt_Player = 0,
	ColAtt_Enemy,
	ColAtt_Candle,
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

	ColliderAttribute GetColliderAttribute() { return static_cast<ColliderAttribute>(mAttribute); }

	ColliderShape GetColliderShape() { return static_cast<ColliderShape>(mShape); }

protected:
	const Uint8 mAttribute;

	const Uint8 mShape;
};
