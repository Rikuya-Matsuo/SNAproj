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

private:
	enum AnimationPattern
	{
		Anim_Stay = 0,
		Anim_DashAttack,
		Anim_DashAttackCompletion,
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

	std::list<const ColliderComponentBase*> mLandingGrounds;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnDetectGround(const ColliderComponentBase * opponent);

	void OnLanding();
};
