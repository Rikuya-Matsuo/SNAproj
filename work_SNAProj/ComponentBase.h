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

	//static void UpdateComponents();

protected:
	//static std::list<ComponentBase *> mComponentList;

	Actor * mOwner;

	// ���̒l�����������������X�V�����B
	int mPriority;
};
