#include "ReelStringEdgeActor.h"
#include "BoxColliderComponent.h"
#include "AutoMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "Player.h"

ReelStringEdgeActor::ReelStringEdgeActor(Player * owner):
	Actor(),
	mOwner(owner),
	mAutoMoveVector(Vector3D(10.0f, 0.0f, 0.0f)),
	mLaunchedXDirection(0)
{
	mCollider = new BoxColliderComponent(this, ColliderAttribute::ColAtt_Detector);

	AABB box;
	box.mMin = Vector3D(-0.5f, -0.5f, -0.5f);
	box.mMax = Vector3D(0.5f, 0.5f, 0.5f);

	mCollider->SetObjectBox(box);
	mCollider->SetActive(false);

	mAutoMoveComp = new AutoMoveComponent(this);

	ClampSpeedComponent * csc = new ClampSpeedComponent(this, Vector3D(250.0f, 0.0f, 0.0f));

	SetAffectGravityFlag(false);

	mScale = 0.2f;

	SetPriority(mOwner->GetPriority() + 100);
}

ReelStringEdgeActor::~ReelStringEdgeActor()
{
}

void ReelStringEdgeActor::UpdateActor0()
{
	Actor::UpdateActor0();

	
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
	}
}

void ReelStringEdgeActor::Launch(bool lookRight)
{
	mPosition = mOwner->GetPosition();

	mLaunchedXDirection = (lookRight) ? 1 : -1;

	Vector3D v = mAutoMoveVector;
	v.x *= mLaunchedXDirection;

	mAutoMoveComp->SetVelocity(v);

	mCollider->SetActive(true);
}
