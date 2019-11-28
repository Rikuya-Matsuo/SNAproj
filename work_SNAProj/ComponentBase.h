#pragma once
#include "Actor.h"
#include "SDL/SDL.h"
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

	void SetActive(bool value) { (!value) ? mFlags |= mNotActiveFlagMask : mFlags &= ~mNotActiveFlagMask; }
	bool GetActiveFlag() const { return !(mFlags & mNotActiveFlagMask); }

protected:
	typedef Uint8 FlagType;

	static const FlagType mNotActiveFlagMask;

	FlagType mFlags;

	Actor * mOwner;

	// この値が小さい方が早く更新される。
	int mPriority;
};
