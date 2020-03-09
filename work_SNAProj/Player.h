﻿#pragma once
#include "Actor.h"

class Mesh;
class InputMoveComponent;
class BoxColliderComponent;
class JumpComponent;
class CompletionMeshActor;
class EnemyBase;
class AnimationEffect;
class NinjaArtsBase;
class BlockHitChecker;

class Player final : public Actor
{
public:
	Player();
	~Player();

	bool GetAliveFlag() const { return mFlags_Player & mAliveFlagMask; }

	char GetLife() const { return mLife; }
	void Damage(unsigned char damageValue) { mLife -= damageValue; }
	void Recover(unsigned char recoverValue) { mLife += recoverValue; }

	bool IsPlayer() const override { return true; }

	static const char mLifeMax;

	friend NinjaArtsBase;

private:
	static const char mDashAttackPower;


	typedef Uint8 FlagType;
	static const FlagType mLandingFlagMask;
	static const FlagType mDetectGroundFlagMask;
	static const FlagType mLookRightFlagMask;
	static const FlagType mImmortalFlagMask;
	static const FlagType mAliveFlagMask;
	static const FlagType mKnockBackFlagMask;
	static const FlagType mJumpInputFlagMask;


	enum AnimationPattern
	{
		Anim_Stay = 0,
		Anim_DashAttack,
		Anim_Run,
		Anim_KnockBack,
		Anim_Invalid
	};

	Mesh * mMesh;

	InputMoveComponent * mInputComponent;

	BoxColliderComponent * mBoxCollider;

	BoxColliderComponent * mGroundChecker;

	BoxColliderComponent * mAttackCollider;

	BlockHitChecker * mHitChecker;

	JumpComponent * mJumpComponent;

	CompletionMeshActor * mCompletionMeshActor;

	NinjaArtsBase * mCurrentCursorNinjaArts;

	char mCurrentAnimation;

	char mLife;

	FlagType mFlags_Player;

	FlagType mPrevFlags_Player;

	std::list<EnemyBase *> mHitList;

	const size_t mHitEffectMass;
	AnimationEffect ** mHitEffects;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnDetectGround();

	void OnLanding();

	void OnLifeRunOut();

	static void OnHitDown(Actor * actor);

	AnimationEffect * FindNonActiveEffect(AnimationEffect ** effArray, size_t mass) const;
};
