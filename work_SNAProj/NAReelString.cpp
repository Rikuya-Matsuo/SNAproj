#include "NAReelString.h"
#include "Player.h"
#include "ReelStringEdgeActor.h"
#include "InputMoveComponent.h"

NAReelString::NAReelString(Player * user):
	NinjaArtsBase(user),
	mRightDashVector(Vector3D(300.0f, 0.0f, 0.0f)),
	mDashVector(Vector3D::zero)
{
	mEdge = new ReelStringEdgeActor(this);
}

NAReelString::~NAReelString()
{
}

void NAReelString::Use()
{
	NinjaArtsBase::Use();

	bool lookRight = mUser->GetLookRightFlag();

	mEdge->Launch(lookRight);

	mUser->SetAffectGravityFlag(false);

	SetAllowJumpFlagOfUser(false);

	GetInputMoveComponent()->SetActive(false);

	mUser->SetMoveVector(Vector3D::zero);
}

void NAReelString::TellEndNinjaArts()
{
	mUser->SetAffectGravityFlag(true);

	SetAllowJumpFlagOfUser(true);

	GetInputMoveComponent()->SetActive(true);

	mIsUsedFlag = false;
}

void NAReelString::TellRunningNinjaArts()
{
	mUser->SetAffectGravityFlag(false);

	Vector3D v = Vector3D::zero;
	if (mEdge->GetReelState() == ReelStringEdgeActor::ReelState::ReelState_Block)
	{
		v = mDashVector;
	}
	mUser->SetMoveVector(v);
}

void NAReelString::CalculateDashVector()
{
	char dir = (mEdge->GetPosition().x > mUser->GetPosition().x) ? 1 : -1;

	mDashVector = mRightDashVector;
	mDashVector.x *= dir;
}
