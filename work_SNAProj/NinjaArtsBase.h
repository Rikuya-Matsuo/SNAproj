﻿#pragma once
#include "BitFlagFunc.h"

class Player;
class InputMoveComponent;

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

	// プレイヤーのブレーキ機能のアクティブ/非アクティブを切り替えられる関数
	void SetActiveBrakeFlagOfUser(bool value);

	InputMoveComponent * GetInputMoveComponent() const;
};
