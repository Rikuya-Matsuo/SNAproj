#pragma once

class Player;

class NinjaArtsBase
{
public:
	NinjaArtsBase(Player * user);
	virtual ~NinjaArtsBase();

	virtual void Use() = 0;

protected:
	bool mIsUsedFlag;

	Player * mUser;
};
