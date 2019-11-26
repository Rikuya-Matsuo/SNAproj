#pragma once
#include "ComponentBase.h"

class ColliderComponentBase : public ComponentBase
{
public:
	ColliderComponentBase(Actor * owner);
	virtual ~ColliderComponentBase();
};
