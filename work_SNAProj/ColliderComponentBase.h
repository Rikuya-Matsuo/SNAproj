#pragma once
#include "ComponentBase.h"

// Unity‚ÅŒ¾‚¤‚Æ‚±‚ë‚ÌTag
enum ColliderAttribute
{
	ColAtt_Player = 0,
	ColAtt_Enemy,
	ColAtt_Candle,
	ColAtt_Invalid
};

class ColliderComponentBase : public ComponentBase
{
public:
	ColliderComponentBase(Actor * owner, ColliderAttribute colAtt);
	virtual ~ColliderComponentBase();

	ColliderAttribute GetColliderAttribute() { return static_cast<ColliderAttribute>(mAttribute); }

protected:
	Uint8 mAttribute;
};
