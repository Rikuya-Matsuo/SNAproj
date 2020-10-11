#include "NinjaArtsBase.h"
#include "Player.h"

NinjaArtsBase::NinjaArtsBase(Player * user):
	mUser(user),
	mIconTexture(nullptr),
	mIconID(-1)
{
}

NinjaArtsBase::~NinjaArtsBase()
{
}

void NinjaArtsBase::Use()
{
	mIsUsedFlag = true;
}

void NinjaArtsBase::CancelNinjaArts()
{
	mIsUsedFlag = false;
}

void NinjaArtsBase::SetAllowJumpFlagOfUser(bool value)
{
	BitFlagFunc::SetFlagByBool(value, mUser->mFlags_Player, Player::mAllowJumpFlagMask);
}

void NinjaArtsBase::SetActiveBrakeFlagOfUser(bool value)
{
	BitFlagFunc::SetFlagByBool(value, mUser->mFlags_Player, Player::mActiveBrakeFlag);
}

InputMoveComponent * NinjaArtsBase::GetInputMoveComponent() const
{
	return mUser->mInputComponent;
}

ClampSpeedComponent* NinjaArtsBase::GetClampSpeedComponent() const
{
	return mUser->mClampSpeedComponent;
}
