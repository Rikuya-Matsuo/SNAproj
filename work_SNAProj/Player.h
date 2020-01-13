#pragma once
#include "Actor.h"

class Mesh;
class InputMoveComponent;
class BoxColliderComponent;
class JumpComponent;
class CompletionMeshActor;

// 大量生成するものではないのでビットフラグは使わない

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

	bool mLandingFlag;

	bool mDetectGroundFlag;

	bool mLookRightFlag;

	char mCurrentAnimation;

	char mLife;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnDetectGround(const ColliderComponentBase * opponent);

	void OnLanding();
};
