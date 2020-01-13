#pragma once
#include "ComponentBase.h"
#include "Collision.h"

// Unityで言うところのTag
enum ColliderAttribute
{
	ColAtt_Player = 0,
	ColAtt_PlayerAttack,
	ColAtt_Enemy,
	ColAtt_EnemyAttack,
	ColAtt_Block,
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

	virtual const AABB * GetWorldBox() const { return nullptr; }

	virtual const Sphere * GetSphere() const { return nullptr; }

	void OnHit(const ColliderComponentBase * col) { mOwner->OnHit(this, col); };
	void OnTouching(const ColliderComponentBase * col) { mOwner->OnTouching(this, col); }
	void OnApart(const ColliderComponentBase * col) { mOwner->OnApart(this, col); }

	// ヒット時に押し返しを受けるかどうかのフラグのセット（デフォルトで真）
	void SetMoveOnHitFlag(bool value) { mMoveOnHitFlag = value; }

	bool GetMoveOnHitFlag() const { return mMoveOnHitFlag; }

	// ヒット時に相手を押すかどうかのフラグ（デフォルトで真）
	void SetPushOnHitFlag(bool value) { mPushOnHitFlag = value; }

	bool GetPushOnHitFlag() const { return mPushOnHitFlag; }

protected:
	const Uint8 mAttribute;

	const Uint8 mShape;

	bool mMoveOnHitFlag;

	bool mPushOnHitFlag;
};
