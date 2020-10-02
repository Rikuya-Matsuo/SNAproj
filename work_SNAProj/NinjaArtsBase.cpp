#include "NinjaArtsBase.h"
#include "Player.h"

NinjaArtsBase::NinjaArtsBase(Player * user):
	mUser(user)
{
}

NinjaArtsBase::~NinjaArtsBase()
{
}

void NinjaArtsBase::Use()
{
	mIsUsedFlag = true;
}

void NinjaArtsBase::SetAllowJumpFlagOfUser(bool value)
{
	BitFlagFunc::SetFlagByBool(value, mUser->mFlags_Player, Player::mAllowJumpFlagMask);
}

InputMoveComponent * NinjaArtsBase::GetInputMoveComponent() const
{
	return mUser->mInputComponent;
}
