#include "NAReelString.h"
#include "Player.h"
#include "ReelStringEdgeActor.h"
#include "InputMoveComponent.h"
#include "ClampSpeedComponent.h"
#include "EnemyBase.h"

NAReelString::NAReelString(Player * user):
	NinjaArtsBase(user),
	mRightDashVector(Vector3D(300.0f, 0.0f, 0.0f)),
	mDashVector(Vector3D::zero),
	mDashSpeed(300.0f)
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

	SetActiveBrakeFlagOfUser(false);

	mUserSpeedLimitRecord = GetClampSpeedComponent()->GetLimit();

	mUser->SetMoveVector(Vector3D::zero);
}

void NAReelString::TellEndNinjaArts()
{
	mUser->SetAffectGravityFlag(true);

	SetAllowJumpFlagOfUser(true);

	GetInputMoveComponent()->SetActive(true);

	GetClampSpeedComponent()->SetLimit(mUserSpeedLimitRecord);

	SetActiveBrakeFlagOfUser(true);

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


	if (mEdge->GetReelState() == ReelStringEdgeActor::ReelState::ReelState_Enemy)
	{
		BringEnemy();
	}
}

void NAReelString::CalculateDashVector()
{
	char dir = (mEdge->GetPosition().x > mUser->GetPosition().x) ? 1 : -1;

	mDashVector = mRightDashVector;
	mDashVector.x *= dir;

	GetClampSpeedComponent()->SetLimit(mDashVector);

	Actor * hitBlock = mEdge->GetHitBlock();
	float diffHeight = hitBlock->GetPosition().z - mUser->GetPosition().z;
	if (diffHeight > 0.0f)
	{
		float distance = hitBlock->GetPosition().x - mUser->GetPosition().x;
		mDashVector.z += diffHeight / distance;
	}
}

void NAReelString::BringEnemy()
{
	Actor * enemy = mEdge->GetHitEnemy();

	const Vector3D & v = mEdge->GetMoveVector();
	enemy->SetMoveVector(v);
}
