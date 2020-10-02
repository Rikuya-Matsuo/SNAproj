#include "NAReelString.h"
#include "Player.h"
#include "ReelStringEdgeActor.h"
#include "InputMoveComponent.h"

NAReelString::NAReelString(Player * user):
	NinjaArtsBase(user)
{
	mEdge = new ReelStringEdgeActor(user, this);
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

	mUser->SetMoveVector(Vector3D::zero);
}
