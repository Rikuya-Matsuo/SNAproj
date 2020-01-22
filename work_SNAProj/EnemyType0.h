#pragma once
#include "EnemyBase.h"

class AnimationChips;
class AutoMoveComponent;
class ColliderComponentBase;
class BoxColliderComponent;
class ClampSpeedComponent;
class Effect;
class BlockHitChecker;

class EnemyType0 : public EnemyBase
{
public:
	EnemyType0();
	~EnemyType0();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	friend EnemyManager;

private:
	static const float mTackleWait;

	typedef Uint8 FlagType;
	static const FlagType mDamageAnimFlagMask;
	static const FlagType mFindPlayerFlagMask;
	static const FlagType mTackleFlagMask;


	const Vector3D mNormalLimit;
	const Vector3D mTackleLimit;
	const Vector3D mKnockBackLimit;

	AutoMoveComponent * mAutoMoveComp;

	AnimationChips * mAnimChips;

	BoxColliderComponent * mBodyCollision;

	BoxColliderComponent * mPlayerDetector;

	BlockHitChecker * mBlockChecker;

	ClampSpeedComponent * mClamper;

	Effect * mFindPlayerEffect;

	Vector3D mVelocity;

	Vector3D mTackleVelocity;

	Vector3D mKnockBackVector;

	float mTackleWaitTimer;

	FlagType mFlags_Enemy0;

	FlagType mPrevFlag_Enemy0;

	void UpdateEnemy0() override;

	void UpdateEnemy1() override;

	void OnFlip() override;

	void OnPressedByplayer(const ColliderComponentBase * caller, Uint8 opponentAtt);

	void DetectPlayer(const ColliderComponentBase * caller, Uint8 opponentAtt);
};

