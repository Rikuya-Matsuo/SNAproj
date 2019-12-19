#pragma once
#include "Actor.h"

class Mesh;
class InputMoveComponent;
class BoxColliderComponent;

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

	bool mLandingFlag;

	bool mDetectGroundFlag;

	std::list<const ColliderComponentBase*> mLandingGrounds;

	void UpdateActor0() override;

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnTouching(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnApart(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;

	void OnDetectGround(const ColliderComponentBase * opponent);
};
