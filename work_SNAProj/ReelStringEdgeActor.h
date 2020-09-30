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

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase* caller, const ColliderComponentBase* opponent) override;

	void Launch(bool lookRight);

private:
	const Vector3D mAutoMoveVector;

	const float mDistanceMaxSq;

	Player * mOwner;
	
	BoxColliderComponent * mCollider;

	AutoMoveComponent * mAutoMoveComp;

	// 打ち出されたX方向。絶対値は1で、-か+で表す。フラグも兼ねており、0の時は打ち出されていないとき。
	char mLaunchedXDirection;

	// 進んだ距離の累計
	Vector3D mDistance;
};
