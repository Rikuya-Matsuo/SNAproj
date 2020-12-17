#pragma once
#include "Actor.h"

class GameScene;

class StageClearBoxActor : public Actor
{
public:
	StageClearBoxActor();
	~StageClearBoxActor();

	void OnHit(const ColliderComponentBase * caller, const ColliderComponentBase * opponent) override;
};
