#pragma once
#include "EnemyBase.h"

class AnimationChips;
class AutoMoveComponent;
class BoxColliderComponent;
class ClampSpeedComponent;
class Effect;
class BlockHitChecker;

class EnemyTest : public EnemyBase
{
public:
	EnemyTest();
	~EnemyTest();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	friend EnemyManager;

private:
	typedef Uint8 FlagType;
	static const FlagType mDamageAnimFlagMask;
	static const FlagType mLDetectGroundFlagMask;
	static const FlagType mRDetectGroundFlagMask;
	static const FlagType mHitWallFlagMask;
	static const FlagType mDetectPlayerFlagMask;
	static const FlagType mDetectWallFlagMask;
	static const FlagType mTackleFlagMask;
	static const FlagType mKnockBackFlagMask;

	static const float mTackleWait;


	const Vector3D mNormalVelocityLimit;

	const Vector3D mTackleVelocityLimit;

	const Vector3D mKnockBackRightVector;

	const Vector3D mKnockBackVecLimit;

	FlagType mFlags_EnemyTest;

	FlagType mPrevFlags_EnemyTest;

	unsigned char mTextureIndex;

	float mDamageAnimTimer;

	float mTackleWaitTimer;

	Vector3D mVelocity;

	Vector3D mTackleVelocity;

	Vector3D mKnockBackVector;

	Vector3D mPlayerDirection;

	Vector3D mWallDerection;

	Vector3D mEffectOffset;

	AnimationChips * mAnimChips;

	Effect * mFindPlayerEffect;

	AutoMoveComponent * mAutoMoveComp;

	BoxColliderComponent * mBodyCollision;

	//BoxColliderComponent * mLGroundDetector;

	//BoxColliderComponent * mRGroundDetector;

	//BoxColliderComponent * mWallDetector;

	BoxColliderComponent * mPlayerDetector;

	ClampSpeedComponent * mClamper;

	BlockHitChecker * mBlockChecker;

	void UpdateEnemy0() override;

	void UpdateEnemy1() override;

	void OnFlip() override;

	void TackleProcess();
};
