#pragma once
#include "Actor.h"

class NAReelString;
class EnemyBase;
class BoxColliderComponent;
class AutoMoveComponent;

class ReelStringEdgeActor final : public Actor
{
public:
	// リールの巻取り状態
	// 何にもヒットしていないときInvalid
	enum ReelState : Uint8
	{
		ReelState_Enemy = 0,
		ReelState_Block,
		ReelState_Invalid
	};

	ReelStringEdgeActor(NAReelString * ninjaArts);
	~ReelStringEdgeActor();

	void UpdateActor1() override;

	void OnHit(const ColliderComponentBase* caller, const ColliderComponentBase* opponent) override;

	void Launch(bool lookRight);

	ReelState GetReelState() const { return mReelState; }

private:
	const Vector3D mAutoMoveVector;

	const float mDistanceMaxSq;

	NAReelString * mNinjaArts;

	EnemyBase * mHitEnemy;

	BoxColliderComponent * mCollider;

	AutoMoveComponent * mAutoMoveComp;

	// 打ち出されたX方向。絶対値は1で、-か+で表す。フラグも兼ねており、0の時は打ち出されていないとき。
	char mLaunchedXDirection : 2;

	ReelState mReelState : 6;

	// 進んだ距離の累計
	Vector3D mDistance;
};
