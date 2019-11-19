#pragma once
#include "Actor.h"
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

protected:
	Actor * mOwner;

	// この値が小さい方が早く更新される。
	int mPriority;
};
