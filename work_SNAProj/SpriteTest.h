﻿#pragma once
#include "Actor.h"

class SpriteTest : public Actor
{
public:
	SpriteTest();
	~SpriteTest();

private:
	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;
};

