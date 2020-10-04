﻿#pragma once
#include "Actor.h"
#include "SDL/SDL.h"
#include "BitFlagFunc.h"
#include <list>

class Actor;

enum UpdateTiming
{
	UpdateTiming_Normal = 0,
	UpdateTiming_AfterAddMoveVector,
	UpdateTiming_Invalid
};

class ComponentBase
{
public:
	ComponentBase(Actor * owner, int priority = 100, UpdateTiming updateTiming = UpdateTiming_Normal);
	virtual ~ComponentBase();

	virtual void Update() = 0;

	Actor * GetOwner() const { return mOwner; }

	int GetPriority() const { return mPriority; }

	void SetActive(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mActiveFlagMask); }
	bool GetActiveFlag() const { return mFlags & mActiveFlagMask; }

	UpdateTiming GetUpdateTiming() const { return static_cast<UpdateTiming>(mUpdateTiming); }

	// 現在のアクティブフラグの状態を記録してから、非アクティブにする
	void Sleep();

	// アクティブフラグをSleep()前の状態に戻す
	void Wake();

protected:
	typedef Uint8 FlagType;

	static const FlagType mActiveFlagMask;
	static const FlagType mActiveFlagBeforeSleepMask;
	static const FlagType mSleepFlagMask;

	const Uint8 mUpdateTiming;

	FlagType mFlags;

	Actor * mOwner;

	// この値が小さい方が早く更新される。
	int mPriority;
};
