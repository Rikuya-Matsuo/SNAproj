#pragma once
#include "Actor.h"

class Mesh;
class InputMoveComponent;
class BoxColliderComponent;
class JumpComponent;

// 大量生成するものではないのでビットフラグは使わない

class Player final : public Actor
{
public:
	Player();
	~Player();

private:
	Mesh * mMesh;

	InputMoveComponent * mInputComponent;

	BoxColliderComponent * mBoxCollider;

	BoxColliderComponent * mGroundChecker;

	JumpComponent * mJumpComponent;

	bool mLandingFlag;

	bool mDetectGroundFlag;

	bool mLookRightFlag;

	std::list<const ColliderComponentBase*> mLandingGrounds;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnDetectGround(const ColliderComponentBase * opponent);

	void OnLanding();
};
