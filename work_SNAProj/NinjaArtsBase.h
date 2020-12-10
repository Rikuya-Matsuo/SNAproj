#pragma once
#include "BitFlagFunc.h"
#include "Player.h"

class Mesh;
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

	virtual void EndNinjaArts();
	
	virtual void OnBeDamaged();

	char GetIconID() const { return mIconID; }

	void SetIconID(char id) { mIconID = id; }

protected:
	bool mIsUsedFlag;

	Player * mUser;

	Texture * mIconTexture;

	char mIconID;

	// ユーザーのジャンプ許可フラグをセットする関数
	void SetAllowJumpFlagOfUser(bool value) { BitFlagFunc::SetFlagByBool(value, mUser->mFlags_Player, Player::mAllowJumpFlagMask); }

	// プレイヤーのブレーキ機能のアクティブ/非アクティブを切り替えられる関数
	void SetActiveBrakeFlagOfUser(bool value) { BitFlagFunc::SetFlagByBool(value, mUser->mFlags_Player, Player::mActiveBrakeFlagMask); }
	
	Mesh * GetMesh() const { return mUser->mMesh; }

	InputMoveComponent * GetInputMoveComponent() const { return mUser->mInputComponent; }

	ClampSpeedComponent * GetClampSpeedComponent() const { return mUser->mClampSpeedComponent; }

	void SetAnimationIndex(Player::AnimationPattern anim) { mUser->mCurrentAnimation = anim; }

	void SetSelfControlAnimationFlag(bool value) { BitFlagFunc::SetFlagByBool(value, mUser->mFlags_Player, Player::mSelfControlAnimationFlagMask); }
};
