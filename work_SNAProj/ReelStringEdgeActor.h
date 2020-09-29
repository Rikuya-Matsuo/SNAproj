#pragma once
#include "Actor.h"

class Player;
class BoxColliderComponent;
class AutoMoveComponent;

class ReelStringEdgeActor : public Actor
{
public:
	ReelStringEdgeActor(Player * owner);
	~ReelStringEdgeActor();

	void UpdateActor0() override;

	void OnHit(const ColliderComponentBase* caller, const ColliderComponentBase* opponent) override;

private:
	const Vector3D mAutoMoveVector;

	Player * mOwner;
	
	BoxColliderComponent * mCollider;

	AutoMoveComponent * mAutoMoveComp;

	bool mLaunchedToRightFlag;
};
