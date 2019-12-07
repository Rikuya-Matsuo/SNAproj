#pragma once
#include "Actor.h"
#include "SDL/SDL.h"
#include "BitFlagFunc.h"
#include <list>

class Actor;

class ComponentBase
{
public:
	ComponentBase(Actor * owner, int priority = 100);
	virtual ~ComponentBase();

	virtual void Update() = 0;

	Actor * GetOwner() const { return mOwner; }

	int GetPriority() const { return mPriority; }

	void SetActive(bool value) { BitFlagFunc::SetFlagByBool(value, mFlags, mActiveFlagMask); }
	bool GetActiveFlag() const { return mFlags & mActiveFlagMask; }

protected:
	typedef Uint8 FlagType;

	static const FlagType mActiveFlagMask;

	FlagType mFlags;

	Actor * mOwner;

	// この値が小さい方が早く更新される。
	int mPriority;
};
