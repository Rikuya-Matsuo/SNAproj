#pragma once
#include "EnemyBase.h"

class AnimationChips;

class EnemyTest : public EnemyBase
{
public:
	EnemyTest();
	~EnemyTest();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

private:
	typedef Uint8 FlagType;
	static const FlagType mDamageAnimFlagMask;


	FlagType mFlags_EnemyTest;

	unsigned char mTextureIndex;

	float mDamageAnimTimer;

	AnimationChips * mAnimChips;

	void UpdateActor0() override;

	void UpdateActor1() override;
};
