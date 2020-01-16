#pragma once
#include "Actor.h"

class Mesh;
class InputMoveComponent;
class BoxColliderComponent;
class JumpComponent;
class CompletionMeshActor;
class EnemyBase;
class Effect;

class Player final : public Actor
{
public:
	Player();
	~Player();

	char GetLife() const { return mLife; }
	void Damage(unsigned char damageValue) { mLife -= damageValue; }
	void Recover(unsigned char recoverValue) { mLife += recoverValue; }

	static const char mLifeMax;

private:
	static const char mDashAttackPower;


	typedef Uint8 FlagType;
	static const FlagType mLandingFlagMask;
	static const FlagType mDetectGroundFlagMask;
	static const FlagType mLookRightFlagMask;
	static const FlagType mImmortalFlagMask;
	static const FlagType mAliveFlagMask;

	enum AnimationPattern
	{
		Anim_Stay = 0,
		Anim_DashAttack,
		Anim_Invalid
	};

	Mesh * mMesh;

	InputMoveComponent * mInputComponent;

	BoxColliderComponent * mBoxCollider;

	BoxColliderComponent * mGroundChecker;

	BoxColliderComponent * mAttackCollider;

	JumpComponent * mJumpComponent;

	CompletionMeshActor * mCompletionMeshActor;

	char mCurrentAnimation;

	char mLife;

	FlagType mFlags_Player;

	FlagType mPrevFlags_Player;

	std::list<EnemyBase *> mHitList;

	const size_t mHitEffectMass;
	Effect ** mHitEffects;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnDetectGround(const ColliderComponentBase * opponent);

	void OnLanding();

	void OnLifeRunOut();

	Effect * FindNonActiveEffect(const Effect ** effArray, size_t mass) const;
};
