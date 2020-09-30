#include "ReelStringEdgeActor.h"
#include "BoxColliderComponent.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "MeshComponent.h"
#include "Player.h"
#include "System.h"
#include <cmath>

ReelStringEdgeActor::ReelStringEdgeActor(Player * owner):
	Actor(),
	mOwner(owner),
	mAutoMoveVector(Vector3D(10.0f, 0.0f, 0.0f)),
	mLaunchedXDirection(0),
	mDistance(Vector3D::zero),
	mDistanceMaxSq(powf(100.0f, 2))
{
	mCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);

	AABB box;
	box.mMin = Vector3D(-0.5f, -0.5f, -0.5f);
	box.mMax = Vector3D(0.5f, 0.5f, 0.5f);

	mCollider->SetObjectBox(box);
	mCollider->SetActive(false);

	mAutoMoveComp = new AutoMoveComponent(this);

	ClampSpeedComponent * csc = new ClampSpeedComponent(this, Vector3D(300.0f, 0.0f, 0.0f));

	Mesh * mesh = System::GetInstance().GetRenderer()->GetMesh("Assets/Board.gpmesh", this);

	MeshComponent * mc = new MeshComponent(this, 500, false);
	mc->SetMesh(mesh);

	SetAffectGravityFlag(false);

	SetActive(false);

	SetVisible(false);

	mScale = 5.0f;

	SetPriority(mOwner->GetPriority() + 100);
}

ReelStringEdgeActor::~ReelStringEdgeActor()
{
}

void ReelStringEdgeActor::UpdateActor1()
{
	Actor::UpdateActor1();

	// 距離の記録
	mDistance += mMoveVector;

	// 現在の進行方向算出
	float velX = mAutoMoveComp->GetVelocity().x;
	char dir = (velX > 0) ? 1 : -1;

	// 一定距離まで行ったら戻る
	// この判定は現在の進行方向とフラグが一致しているときのみ行う
	if (mLaunchedXDirection == dir && mDistance.LengthSq() >= mDistanceMaxSq)
	{
		Vector3D v = mAutoMoveComp->GetVelocity();
		v.x *= -1;
	
		mAutoMoveComp->SetVelocity(v);
	}
}

void ReelStringEdgeActor::OnHit(const ColliderComponentBase* caller, const ColliderComponentBase* opponent)
{
	ColliderAttribute oppAtt = opponent->GetColliderAttribute();

	if (oppAtt == ColliderAttribute::ColAtt_Enemy)
	{

	}
	else if (oppAtt == ColliderAttribute::ColAtt_Block)
	{
		mAutoMoveComp->SetVelocity(Vector3D::zero);

		mOwner->SetPosition(mPosition);

		mCollider->SetActive(false);

		mLaunchedXDirection = 0;

		SetActive(false);

		SetVisible(false);
	}
	else if (oppAtt == ColliderAttribute::ColAtt_Player)
	{
		// 現在の進行方向
		float velX = mAutoMoveComp->GetVelocity().x;
		char dir = (velX > 0) ? 1 : -1;

		// フラグと現在の進行方向が逆なら、戻ってる最中である。
		if (mLaunchedXDirection == -dir)
		{
			SetActive(false);
		}
	}
}

void ReelStringEdgeActor::Launch(bool lookRight)
{
	mPosition = mOwner->GetPosition();

	mDistance = mMoveVector = Vector3D::zero;

	mLaunchedXDirection = (lookRight) ? 1 : -1;

	Vector3D v = mAutoMoveVector;
	v.x *= mLaunchedXDirection;

	mAutoMoveComp->SetVelocity(v);

	mCollider->SetActive(true);

	SetActive(true);

	SetVisible(true);
}
