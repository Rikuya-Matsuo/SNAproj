#pragma once
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

protected:
	bool mIsUsedFlag;

	Player * mUser;

	// ユーザーのジャンプ許可フラグをセットする関数
	void SetAllowJumpFlagOfUser(bool value);

	InputMoveComponent * GetInputMoveComponent() const;
};
