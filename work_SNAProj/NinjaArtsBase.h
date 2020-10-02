﻿#pragma once
#include "BitFlagFunc.h"

class Player;
class InputMoveComponent;
class ClampSpeedComponent;

class NinjaArtsBase
{
public:
	NinjaArtsBase(Player * user);
	virtual ~NinjaArtsBase();

	virtual void Use();

	bool IsUsed() const { return mIsUsedFlag; }

	Player * GetUserPlayer() const { return mUser; }
	
protected:
	bool mIsUsedFlag;

	Player * mUser;

	// ユーザーのジャンプ許可フラグをセットする関数
	void SetAllowJumpFlagOfUser(bool value);

	InputMoveComponent * GetInputMoveComponent() const;

	ClampSpeedComponent * GetClampSpeedComponent() const;
};
