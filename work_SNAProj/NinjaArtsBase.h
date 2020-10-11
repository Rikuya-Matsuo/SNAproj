#pragma once
#include "BitFlagFunc.h"

class Player;
class InputMoveComponent;
class ClampSpeedComponent;
class Texture;

class NinjaArtsBase
{
public:
	NinjaArtsBase(Player * user);
	virtual ~NinjaArtsBase();

	virtual void Use();

	bool IsUsed() const { return mIsUsedFlag; }

	Player * GetUserPlayer() const { return mUser; }

	Texture * GetIconTexture() const { return mIconTexture; }

	virtual void CancelNinjaArts();
	
	char GetIconID() const { return mIconID; }

	void SetIconId(char id) { mIconID = id; }

protected:
	bool mIsUsedFlag;

	Player * mUser;

	Texture * mIconTexture;

	char mIconID;

	// ユーザーのジャンプ許可フラグをセットする関数
	void SetAllowJumpFlagOfUser(bool value);

	// プレイヤーのブレーキ機能のアクティブ/非アクティブを切り替えられる関数
	void SetActiveBrakeFlagOfUser(bool value);

	InputMoveComponent * GetInputMoveComponent() const;

	ClampSpeedComponent * GetClampSpeedComponent() const;
};
