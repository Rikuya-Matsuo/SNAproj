#pragma once
#include "Actor.h"

class SpriteActor : public Actor
{
public:
	SpriteActor(const char * texPath);
	~SpriteActor();

	void UpdateActor1() override;
};
