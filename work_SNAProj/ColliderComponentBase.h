#pragma once
#include "ComponentBase.h"
#include "Collision.h"

// Unity�Ō����Ƃ����Tag
enum ColliderAttribute
{
	ColAtt_Player = 0,
	ColAtt_Enemy,
	ColAtt_Candle,
	ColAtt_Block,
	ColAtt_Detector,		// �g���K�[�B�ڋߌ��m���u�B�q�b�g���̉����Ԃ��Ȃ��B
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

	void OnHit(ColliderAttribute colAtt) { mOwner->OnHit(this, colAtt); };

protected:
	const Uint8 mAttribute;

	const Uint8 mShape;
};
