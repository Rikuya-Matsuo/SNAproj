#pragma once

class Player;

class NinjaArtsBase
{
public:
	NinjaArtsBase(Player * user);
	virtual ~NinjaArtsBase();

	virtual void Use();

	bool IsUsed() const { return mIsUsedFlag; }

protected:
	bool mIsUsedFlag;

	Player * mUser;
};
