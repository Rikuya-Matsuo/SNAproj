#pragma once
#include "EnemyBase.h"

class AnimationChips;
class AutoMoveComponent;
class BoxColliderComponent;

class EnemyTest : public EnemyBase
{
public:
	EnemyTest();
	~EnemyTest();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

private:
	typedef Uint8 FlagType;
	static const FlagType mDamageAnimFlagMask;
	static const FlagType mLDetectGroundFlagMask;
	static const FlagType mRDetectGroundFlagMask;


	FlagType mFlags_EnemyTest;

	unsigned char mTextureIndex;

	float mDamageAnimTimer;

	Vector3D mVelocity;

	AnimationChips * mAnimChips;

	AutoMoveComponent * mAutoMoveComp;

	BoxColliderComponent * mLDetector;

	BoxColliderComponent * mRDetector;

	void UpdateEnemy0() override;

	void UpdateEnemy1() override;
};
