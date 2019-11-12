#pragma once
#include "Actor.h"

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

	// ���̒l�����������������X�V�����B
	int mPriority;
};
